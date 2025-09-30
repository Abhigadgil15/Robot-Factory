#ifndef __ORDERS_H__
#define __ORDERS_H__

#include <string>

#include <string.h>


// #define CLI_RAND_SEED       1234
// #define SVR_RAND_SEED       4321

// #define NUM_REQ             3
// #define MAX_REQUESTER_LEN   32

// #define REQUEST_BUF_SIZE    46
// #define RESPONSE_BUF_SIZE   14


class Orders {
    int customer_id;     // customer id
    int order_number;    // number of orders issued so far by this customer
    int robot_type;      // 0 = regular, 1 = special

public:
    Orders() : customer_id(0), order_number(0), robot_type(0) {}

    int GetCustomerID()  const { return customer_id; }
    int GetOrderNumber() const { return order_number; }
    int GetRobotType()   const { return robot_type;  }

    void SetOrder(int cust_id, int ord_num, int rob_type);

    int  Marshal(char *buffer) ;

    void Unmarshal(char *buffer);

    void PrintOrder() ;
};

// ---------- Robot (server → client) ----------
class Robot {
    int customer_id;   // copied from order
    int order_number;  // copied from order
    int robot_type;    // copied from order
    int engineer_id;   // id of engineer who created the robot
    int expert_id;     // id of expert for special module (-1 if none)

public:
    Robot() : customer_id(0), order_number(0),
              robot_type(0), engineer_id(0), expert_id(-1) {}

    // Getters
    int GetCustomerID() const { return customer_id; }
    int GetOrderNumber() const { return order_number; }
    int GetRobotType()  const { return robot_type; }
    int GetEngineerID() const { return engineer_id; }
    int GetExpertID()   const { return expert_id;   }

    // Set all fields
    void SetRobot(int cust_id, int ord_num, int rob_type,
                  int eng_id, int exp_id);

    // Serialize to fixed-size buffer for send()
    int  Marshal(char *buffer) ;

    // Deserialize from received buffer
    void Unmarshal(char *buffer);

    void PrintRobot();
};

#endif // end of #ifndef __MESSAGE_H__
