#pragma once
#include <string>
#include "Order.h"

class ClientStub {
private:
    int sockfd;          // TCP socket
    char buffer[64];     // reuse buffer for marshalling

public:
    ClientStub() : sockfd(-1) {}

    // Initialize connection to server
    bool Init(const std::string &ip, int port);

    

    // Send order to server and receive robot
    Robot Order(int customer_id, int order_number, int robot_type);
};
