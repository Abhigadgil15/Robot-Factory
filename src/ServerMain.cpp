#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

#include "ServerStub.h"
#include "Order.h"

std::atomic<int> totalOrdersProcessed(0);
std::mutex print_mutex; // synchronize log printing

void engineerThread(int client_sock, int eng_id) {
    ServerStub server;
    if (!server.Init(client_sock)) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cerr << "[Server] Failed to initialize ServerStub for client socket "
                  << client_sock << std::endl << std::endl;
        close(client_sock);
        return;
    }

    while (true) {
        Orders order = server.ReceiveOrder();
        if (order.GetCustomerID() == -1) break;

        Robot robot;
        int exp_id = -1;
        robot.SetRobot(order.GetCustomerID(), order.GetOrderNumber(),
                       order.GetRobotType(), eng_id, exp_id);

        server.ShipRobot(robot);

        int currentTotal = ++totalOrdersProcessed;

        // Structured log: Received → Sent → blank line
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "[Received] customer_id=" << order.GetCustomerID()
                      << ", order_number=" << order.GetOrderNumber()
                      << ", robot_type=" << order.GetRobotType()
                      << std::endl;

            std::cout << "[Sent] customer_id=" << robot.GetCustomerID()
                      << ", order_number=" << robot.GetOrderNumber()
                      << ", engineer_id=" << eng_id
                      << ", expert_id=" << exp_id
                      << ", total_processed=" << currentTotal
                      << std::endl << std::endl;
        }
    }

    close(client_sock);
    {
        std::lock_guard<std::mutex> lock(print_mutex);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        return 0;
    }

    int port = atoi(argv[1]);
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) { perror("socket"); return 1; }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind"); return 1;
    }

    if (listen(listen_sock, 8) < 0) {
        perror("listen"); return 1;
    }

    std::cout << "[Server] Listening on port " << port << std::endl << std::endl;

    std::vector<std::thread> engineerThreads;
    int eng_counter = 0;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(listen_sock, (sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) { perror("accept"); continue; }

        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "[Server] Accepted new client connection (socket " 
                      << client_sock << ", engineer_id=" << eng_counter << ")"
                      << std::endl << std::endl;
        }

        engineerThreads.emplace_back(engineerThread, client_sock, eng_counter++);
    }

    for (auto &t : engineerThreads) t.join();
    close(listen_sock);
    return 0;
}
