#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include "ServerStub.h"
#include "Order.h"

#define BACKLOG 8

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [port]" << std::endl;
        return 0;
    }

    int port = atoi(argv[1]);

    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }

    listen(listen_sock, BACKLOG);
    std::cout << "Waiting for client to connect..." << std::endl;

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(listen_sock, (sockaddr*)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("accept");
        return 1;
    }

    // Initialize server stub
    ServerStub server;
    server.Init(client_sock);

    std::srand(time(nullptr));

    for (int i = 0; i < 5; i++) {
        // Receive order
        Orders order = server.ReceiveOrder();

        std::cout << "[Received] Order {"
                  << " customer_id=" << order.GetCustomerID()
                  << ", order_number=" << order.GetOrderNumber()
                  << ", robot_type=" << order.GetRobotType()
                  << " }" << std::endl;

        // Create robot based on order
        Robot robot;
        int eng_id  = std::rand() % 1000;
        int exp_id  = (order.GetRobotType() == 1) ? std::rand() % 500 : -1;

        robot.SetRobot(order.GetCustomerID(), order.GetOrderNumber(),
                       order.GetRobotType(), eng_id, exp_id);

        // Send robot
        server.ShipRobot(robot);

        std::cout << "[Sent] Robot {"
                  << " customer_id=" << robot.GetCustomerID()
                  << ", order_number=" << robot.GetOrderNumber()
                  << ", robot_type=" << robot.GetRobotType()
                  << ", engineer_id=" << robot.GetEngineerID()
                  << ", expert_id=" << robot.GetExpertID()
                  << " }" << std::endl;
    }

    close(listen_sock);
    return 0;
}
