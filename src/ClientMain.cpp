#include <iostream>
#include <cstdlib>
#include <ctime>
#include "ClientStub.h"
#include "Order.h"

#define SHORT_MAX 65536

// int main(int argc, char* argv[]) {
//     if (argc < 3) {
//         std::cout << "Usage: " << argv[0] << " [ip] [port]" << std::endl;
//         return 0;
//     }

//     std::string server_ip = argv[1];
//     int port = atoi(argv[2]);

//     // Create client stub instance
//     ClientStub client;
//     if (!client.Init(server_ip, port)) {
//         std::cerr << "Failed to connect to server!" << std::endl;
//         return 1;
//     }

//     std::srand(time(nullptr));

//     for (unsigned int i = 0; i < 5; i++) {
//         int cust_id = abs(std::rand() % SHORT_MAX);
//         int ord_num = i;
//         int rob_type = std::rand() % 4;

//         // Send order and receive robot using stub
//         Robot robot = client.Order(cust_id, ord_num, rob_type);

//         std::cout << "[Received] Robot {"
//                   << " customer_id=" << robot.GetCustomerID()
//                   << ", order_number=" << robot.GetOrderNumber()
//                   << ", robot_type=" << robot.GetRobotType()
//                   << ", engineer_id=" << robot.GetEngineerID()
//                   << ", expert_id=" << robot.GetExpertID()
//                   << " }" << std::endl;
//     }

//     return 0;
// }

#include <iostream>
#include <vector>
#include <thread>
#include "CustomerThread.h"

std::mutex print_mutex;

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cout << "Usage: " << argv[0] 
                  << " [ip addr] [port #] [# customers] [# orders] [robot type]" << std::endl;
        return 0;
    }

    std::string server_ip = argv[1];
    int port = std::atoi(argv[2]);
    int num_customers = std::atoi(argv[3]);
    int num_orders = std::atoi(argv[4]);
    int robot_type = std::atoi(argv[5]);


    // Create customer threads
    std::vector<std::shared_ptr<CustomerThread>> customers;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_customers; i++) {
        auto cust_ptr = std::make_shared<CustomerThread>(i, num_orders, robot_type, server_ip, port);
        customers.push_back(cust_ptr);
        threads.push_back(std::thread(&CustomerThread::run, cust_ptr.get()));
    }

    for (auto &t : threads){ 
        t.join();
    }

        return 0;
}
