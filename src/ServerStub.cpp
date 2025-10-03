#include "ServerStub.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>

// Prototypes from existing Communications.cpp
bool part_3_server_recv_unmarshal(Orders &order, char *buffer, int sockfd);
void part_3_server_marshal_send(Robot &robot, char *buffer, int sockfd);

Orders ServerStub::ReceiveOrder() {
    Orders order;
    if (!part_3_server_recv_unmarshal(order, buffer, sockfd)) {
        order.SetOrder(-1,-1,-1);
    }
    return order;
}

void ServerStub::ShipRobot(const Robot &robot) {
    part_3_server_marshal_send(const_cast<Robot&>(robot), buffer, sockfd);
}
