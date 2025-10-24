#include "ClientThread.h"
#include "Messages.h"

#include <iostream>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::ThreadBody(std::string ip, int port, int id, int requests, int type) {
	customer_id = id;
	num_requests = requests;
	request_type = type;
	if (!stub.Init(ip, port)) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}

 if (request_type == 3) {
        for (int cid = 0; cid < num_requests; cid++) {
            CustomerRequest request;
            request.SetRequest(cid, -1, 2);  

            timer.Start();
            RobotInfo robot = stub.Order(request);  
    		CustomerRecord record;
    		record.SetRecord(robot.GetCustomerId(), robot.GetOrderNumber()); 
            timer.EndAndMerge();
            if (record.IsValid() && record.GetCustomerId() != -1) {
                std::cout << record.GetCustomerId() << "\t" << record.GetLastOrder() << std::endl;
            }
        }
        return; // done scanning
    }
	
	for (int i = 0; i < num_requests; i++) {
		CustomerRequest request;
		RobotInfo robot;
		request.SetRequest(customer_id, i, request_type);

		timer.Start();
		robot = stub.Order(request);
		timer.EndAndMerge();

		if (!robot.IsValid()) {
			std::cout << "Invalid robot " << customer_id << std::endl;
			break;	
		} 
	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;	
}

