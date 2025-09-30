#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "ServerStub.h"
#include "Order.h"

#define BACKLOG 8
#define NUM_WORKERS 8  // Number of worker threads

// Thread-safe queue for orders
std::queue<std::pair<int, Orders>> orderQueue; // pair<client_sock, order>
std::mutex queue_mtx;
std::condition_variable cv;

// Atomic counter for total orders processed
std::atomic<int> totalOrdersProcessed(0);

// Worker thread function
void worker() {
    ServerStub server;
    while (true) {
        std::pair<int, Orders> job;
        {
            std::unique_lock<std::mutex> lock(queue_mtx);
            cv.wait(lock, [] { return !orderQueue.empty(); });
            job = orderQueue.front();
            orderQueue.pop();
        }

        int client_sock = job.first;
        Orders order = job.second;

        // If order is a "disconnect" signal
        if (order.GetCustomerID() == -1) continue;

        // Process order
        Robot robot;
        int eng_id = std::rand() % 1000;
        int exp_id = (order.GetRobotType() == 1) ? std::rand() % 500 : -1;

        robot.SetRobot(order.GetCustomerID(), order.GetOrderNumber(),
                       order.GetRobotType(), eng_id, exp_id);

        ServerStub tmp;
        tmp.Init(client_sock);
        tmp.ShipRobot(robot);

        int currentTotal = ++totalOrdersProcessed;

        std::cout << "[Server] Processed order " << currentTotal
                  << " | customer_id=" << robot.GetCustomerID()
                  << ", order_number=" << robot.GetOrderNumber() << std::endl;
    }
}

void clientReceiver(int client_sock) {
    ServerStub server;
    server.Init(client_sock);

    while (true) {
        Orders order = server.ReceiveOrder();
        if (order.GetCustomerID() == -1) break; 

        {
            std::lock_guard<std::mutex> lock(queue_mtx);
            orderQueue.push({client_sock, order});
        }
        cv.notify_one(); // wake a worker
    }

    close(client_sock);
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

    listen(listen_sock, BACKLOG);
    std::cout << "Server listening on port " << port << std::endl;

    // Start worker threads
    std::vector<std::thread> workers;
    for (int i = 0; i < NUM_WORKERS; i++) {
        workers.emplace_back(worker);
    }

    // Accept clients and spawn receiver threads
    std::vector<std::thread> clientThreads;
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(listen_sock, (sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) { perror("accept"); continue; }

        clientThreads.emplace_back(clientReceiver, client_sock);
    }

    for (auto &t : workers) t.join();
    for (auto &t : clientThreads) t.join();
    close(listen_sock);

    return 0;
}
