#include "CustomerThread.h"
#include <iostream>
#include <cstdlib>

CustomerThread::CustomerThread(int cust_id, int n_orders, int r_type,
                               const std::string &ip, int port)
    : customer_id(cust_id), num_orders(n_orders), robot_type(r_type),
      server_ip(ip), server_port(port) {}

void CustomerThread::run() {
    // Initialize connection
    if (!client.Init(server_ip, server_port)) {
        std::cerr << "Customer " << customer_id << " failed to connect to server.\n";
        return;
    }

    for (int i = 0; i < num_orders; i++) {
        Robot robot = client.Order(customer_id, i, robot_type);

        std::cout << "[Customer " << customer_id << "] Received Robot {"
                  << " customer_id=" << robot.GetCustomerID()
                  << ", order_number=" << robot.GetOrderNumber()
                  << ", robot_type=" << robot.GetRobotType()
                  << ", engineer_id=" << robot.GetEngineerID()
                  << ", expert_id=" << robot.GetExpertID()
                  << " }" << std::endl;
    }

    // Connection will be closed automatically in ClientStub destructor if implemented
}

// void CustomerThread::start() {
//     thread_obj = std::thread(&CustomerThread::run, this);
// }

// void CustomerThread::join() {
//     if (thread_obj.joinable())
//         thread_obj.join();
// }
