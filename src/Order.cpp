#include <iostream>
#include <string>
#include <cstring>
#include "Order.h"

// ------------------ Order ------------------
void Orders::SetOrder(int cust_id, int ord_num, int rob_type) {
    customer_id = cust_id;
    order_number = ord_num;
    robot_type = rob_type;
}

void Orders::PrintOrder()  {
    std::cout << "\tORDER  - customer_id: " << customer_id
              << "; order_number: " << order_number
              << "; robot_type: "  << robot_type
              << std::endl;
}

// ------------------ Robot ------------------
void Robot::SetRobot(int cust_id, int ord_num, int rob_type,
                     int eng_id, int exp_id) {
    customer_id = cust_id;
    order_number = ord_num;
    robot_type  = rob_type;
    engineer_id = eng_id;
    expert_id   = exp_id;
}

void Robot::PrintRobot()  {
    std::cout << "\tROBOT  - customer_id: " << customer_id
              << "; order_number: " << order_number
              << "; robot_type: "  << robot_type
              << "; engineer_id: " << engineer_id
              << "; expert_id: "   << expert_id
              << std::endl;
}



