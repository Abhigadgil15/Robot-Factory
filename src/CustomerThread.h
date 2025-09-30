#pragma once
#include <thread>
#include "ClientStub.h"
#include "Order.h"

class CustomerThread {
private:
    int customer_id;
    int num_orders;
    int robot_type;
    std::string server_ip;
    int server_port;
    ClientStub client;
    std::thread thread_obj;

    // The actual function executed by the thread

public:
    CustomerThread(int cust_id, int n_orders, int r_type,
                   const std::string &ip, int port);
    void run();


    // Start the thread
    // void start();

    // // Wait for thread completion
    // void join();
};
