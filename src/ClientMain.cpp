#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "Order.h"  

#define BUFFER_SIZE 64
#define SHORT_MAX   65536

// Prototype from Communications.cpp
void part_3_client_marshal_send_recv_unmarshal(Order &order, Robot &robot,
                                               char *buffer, int sockfd);

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in addr;
    int port;
    char buffer[BUFFER_SIZE];

    if (argc < 3) {
        std::cout << "Usage\n" << argv[0] << " [ip] [port]" << std::endl;
        return 0;
    }
    port = atoi(argv[2]);

/******************************************************************************
 * Initializing the network connection
******************************************************************************/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR: failed to create socket");
        return 0;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("ERROR: failed to connect");
        return 0;
    }

/******************************************************************************
 * Client loop: create orders and receive robots
******************************************************************************/
    std::srand(time(nullptr));
	for (unsigned int i = 0; i < 5; i++) {   // send 5 orders for demo
    	Order order;
    	Robot robot;

    // Use setter instead of direct access
    	int cust_id = abs(std::rand() % SHORT_MAX);
    	int ord_num = i;
    	int rob_type = std::rand() % 4;

    	order.SetOrder(cust_id, ord_num, rob_type);

    	std::cout << "[Sending ] Order {"
              << " customer_id=" << order.GetCustomerID()
              << ", order_number=" << order.GetOrderNumber()
              << ", robot_type=" << order.GetRobotType()
              << " }" << std::endl;

    // Marshal/Send/Recv/Unmarshal
    	part_3_client_marshal_send_recv_unmarshal(order, robot, buffer, sockfd);

    	std::cout << "[Received] Robot {"
              << " customer_id=" << robot.GetCustomerID()
              << ", order_number=" << robot.GetOrderNumber()
              << ", robot_type=" << robot.GetRobotType()
              << ", engineer_id=" << robot.GetEngineerID()
              << ", expert_id=" << robot.GetExpertID()
              << " }" << std::endl;
}


    close(sockfd);
    return 1;
}
