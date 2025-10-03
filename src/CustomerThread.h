#pragma once
#include <thread>
#include "ClientStub.h"
#include "Order.h"
#include <mutex>     // needed for std::mutex
#include <vector>    // needed for std::vector
#include <string>    // needed for std::string

extern std::vector<long long> all_latencies;  
extern std::mutex print_mutex;  

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
};
