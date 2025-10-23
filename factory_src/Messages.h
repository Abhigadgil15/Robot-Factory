#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>

class RobotOrder {
private:
	int customer_id;
	int order_number;
	int robot_type;

public:
	RobotOrder();
	void operator = (const RobotOrder &order) {
		customer_id = order.customer_id;
		order_number = order.order_number;
		robot_type = order.robot_type;
	}
	void SetOrder(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRobotType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class RobotInfo {
private:
    int customer_id;
    int order_number;
    int request_type; // changed from robot_type
    int engineer_id;
    int admin_id;     // changed from expert_id

public:
    RobotInfo();

    void operator = (const RobotInfo &info) {
        customer_id = info.customer_id;
        order_number = info.order_number;
        request_type = info.request_type;
        engineer_id = info.engineer_id;
        admin_id = info.admin_id;
    }

    void SetInfo(int cid, int order_num, int req_type, int engid, int admid);
    void CopyOrder(RobotOrder order);
    void SetEngineerId(int id);
    void SetAdminId(int id); // changed from SetExpertId

    int GetCustomerId();
    int GetOrderNumber();
    int GetRequestType(); // changed from GetRobotType
    int GetEngineerId();
    int GetAdminId();     // changed from GetExpertId

    int Size();

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    bool IsValid();

    void Print();
};

class CustomerRequest {
private:
	int customer_id;
	int order_number;
	int request_type;

public:
	CustomerRequest();
	void operator = (const CustomerRequest &request) {
		customer_id = request.customer_id;
		order_number = request.order_number;
		request_type = request.request_type;
	}
	void SetRequest(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRequestType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};


struct MapOp {
    int opcode; // operation code: 1 - update value
    int arg1;   // customer_id to apply the operation
    int arg2;   // parameter for the operation
};

class CustomerRecord {
	private:
		int customer_id;
		int last_order;
	public:
		CustomerRecord();
		void operator = (const CustomerRecord &record) {
			customer_id = record.customer_id;
			last_order = record.last_order;
		}
		void SetRecord(int cid, int order_num);
		int GetCustomerId();
		int GetLastOrder();
		int Size();
		void Marshal(char *buffer);
		void Unmarshal(char *buffer);

		bool IsValid();
		void Print();
		
};



#endif // #ifndef __MESSAGES_H__
