#pragma once
#include "Order.h"

class ServerStub {
private:
    int sockfd;          // Connected socket to client
    char buffer[64];     // Reusable buffer for marshalling

public:
    ServerStub() : sockfd(-1) {}

    // Initialize with connected socket
    bool Init(int connected_sockfd) { sockfd = connected_sockfd;
    return true; }

    // Receive order from client
    Orders ReceiveOrder();

    // Send robot info to client
    void ShipRobot(const Robot &robot);
};
