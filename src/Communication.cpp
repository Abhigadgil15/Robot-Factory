#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include "Order.h"

// ------------------ Order ------------------
int Order::Marshal(char *buffer)
{
    int offset = 0;

    uint32_t net_customer_id = htonl(customer_id);
    uint32_t net_order_number = htonl(order_number);
    uint32_t net_robot_type = htonl(robot_type);

    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);

    memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
    offset += sizeof(net_order_number);

    memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
    offset += sizeof(net_robot_type);

    return offset;
}

void Order::Unmarshal(char *buffer)
{
    int offset = 0;
    uint32_t net_customer_id, net_order_number, net_robot_type;

    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);

    memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
    offset += sizeof(net_order_number);

    memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type));
    offset += sizeof(net_robot_type);

    customer_id = ntohl(net_customer_id);
    order_number = ntohl(net_order_number);
    robot_type = ntohl(net_robot_type);
}

// ------------------ Robot ------------------
int Robot::Marshal(char *buffer)
{
    int offset = 0;

    uint32_t net_customer_id = htonl(customer_id);
    uint32_t net_order_number = htonl(order_number);
    uint32_t net_robot_type = htonl(robot_type);
    uint32_t net_engineer_id = htonl(engineer_id);
    uint32_t net_expert_id = htonl(expert_id);

    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);

    memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
    offset += sizeof(net_order_number);

    memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
    offset += sizeof(net_robot_type);

    memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
    offset += sizeof(net_engineer_id);

    memcpy(buffer + offset, &net_expert_id, sizeof(net_expert_id));
    offset += sizeof(net_expert_id);

    return offset;
}

void Robot::Unmarshal(char *buffer)
{
    int offset = 0;
    uint32_t net_customer_id, net_order_number, net_robot_type, net_engineer_id, net_expert_id;

    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);

    memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
    offset += sizeof(net_order_number);

    memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type));
    offset += sizeof(net_robot_type);

    memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id));
    offset += sizeof(net_engineer_id);

    memcpy(&net_expert_id, buffer + offset, sizeof(net_expert_id));
    offset += sizeof(net_expert_id);

    customer_id = ntohl(net_customer_id);
    order_number = ntohl(net_order_number);
    robot_type = ntohl(net_robot_type);
    engineer_id = ntohl(net_engineer_id);
    expert_id = ntohl(net_expert_id);
}

// ------------------ Client / Server helpers ------------------
void part_3_client_marshal_send_recv_unmarshal(Order &order, Robot &robot,
                                               char *buffer, int sockfd)
{
    int order_size = order.Marshal(buffer);

    if (send(sockfd, buffer, order_size, 0) != order_size)
    {
        perror("client send");
        return;
    }

    int n = recv(sockfd, buffer, 64, 0);
    if (n <= 0)
    {
        perror("client recv");
        return;
    }

    robot.Unmarshal(buffer);
}

void part_3_server_recv_unmarshal(Order &order, char *buffer, int sockfd)
{
    int n = recv(sockfd, buffer, 64, 0);
    if (n <= 0)
    {
        perror("server recv");
        return;
    }

    order.Unmarshal(buffer);
}

void part_3_server_marshal_send(Robot &robot, char *buffer, int sockfd)
{
    int robot_size = robot.Marshal(buffer);

    if (send(sockfd, buffer, robot_size, 0) != robot_size)
    {
        perror("server send");
    }
}
