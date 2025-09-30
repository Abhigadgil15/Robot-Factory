#include <iostream>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "Order.h"   

#define BACKLOG 8
#define BUFFER_SIZE 64

// Prototypes (defined in Communications.cpp)
void part_3_server_recv_unmarshal(Order &order, char *buffer, int sockfd);
void part_3_server_marshal_send(Robot &robot, char *buffer, int sockfd);

int main(int argc, char *argv[])
{
    int sockfd, newfd;
    struct sockaddr_in my_addr, addr;
    unsigned int addr_size = sizeof(addr);
    int port;
    char buffer[BUFFER_SIZE];

    if (argc < 2) {
        std::cout << "Usage\n" << argv[0] << " [port]" << std::endl;
        return 0;
    }

    port = atoi(argv[1]);

    /******************************************************************************
     * Init network
    ******************************************************************************/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR: failed to create socket");
        return 0;
    }
    memset(&my_addr, '\0', sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("ERROR: failed to bind");
        return 0;
    }

    listen(sockfd, BACKLOG);
    std::cout << "Waiting for client to connect..." << std::endl;

    newfd = accept(sockfd, (struct sockaddr *)&addr, &addr_size);
    if (newfd < 0) {
        perror("ERROR: failed to accept");
        return 0;
    }

    /******************************************************************************
     * Server loop: receive orders and send robots
    ******************************************************************************/
    std::srand(time(nullptr));

    for (int i = 0; i < 5; i++) {
        Order order;
        Robot robot;

        // 1. Receive and unmarshal order
        part_3_server_recv_unmarshal(order, buffer, newfd);

        std::cout << "[Received] Order {"
                  << " customer_id=" << order.GetCustomerID()
                  << ", order_number=" << order.GetOrderNumber()
                  << ", robot_type=" << order.GetRobotType()
                  << " }" << std::endl;

        // 2. Create a robot based on the order
        int cust_id = order.GetCustomerID();
        int ord_num = order.GetOrderNumber();
        int rob_type = order.GetRobotType();
        int eng_id  = std::rand() % 1000;   // assign random engineer
        int exp_id  = (rob_type == 1) ? (std::rand() % 500) : -1;

        robot.SetRobot(cust_id, ord_num, rob_type, eng_id, exp_id);

        std::cout << "[Sending ] Robot {"
                  << " customer_id=" << robot.GetCustomerID()
                  << ", order_number=" << robot.GetOrderNumber()
                  << ", robot_type=" << robot.GetRobotType()
                  << ", engineer_id=" << robot.GetEngineerID()
                  << ", expert_id=" << robot.GetExpertID()
                  << " }" << std::endl;

        // 3. Marshal and send robot
        part_3_server_marshal_send(robot, buffer, newfd);
    }

    close(sockfd);
    return 1;
}
