#include "ClientStub.h"
#include "Order.h"
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
// Prototype from your existing Communications.cpp
void part_3_client_marshal_send_recv_unmarshal(Orders &order, Robot &robot,
                                               char *buffer, int sockfd);

bool ClientStub::Init(const std::string &ip, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ClientStub: failed to create socket");
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("ClientStub: failed to connect");
        return false;
    }
    return true;
}

Robot ClientStub::Order(int customer_id, int order_number, int robot_type) {
    Orders order;
    Robot robot;
    order.SetOrder(customer_id, order_number, robot_type);

    part_3_client_marshal_send_recv_unmarshal(order, robot, buffer, sockfd);
    return robot;
}
