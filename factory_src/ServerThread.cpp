#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"
#include <map>


std :: map<int,int> customer_record;

RobotInfo RobotFactory::CreateRegularRobot(CustomerRequest request, int engineer_id) {
	RobotInfo robot;
	robot.CopyRequest(request);
	robot.SetEngineerId(engineer_id);
	robot.SetAdminId(-1);
	return robot;
}

// RobotInfo RobotFactory::CreateSpecialRobot(RobotOrder order, int engineer_id) {
// 	RobotInfo robot;
// 	robot.CopyOrder(order);
// 	robot.SetEngineerId(engineer_id);
// 	std::promise<RobotInfo> prom;
// 	std::future<RobotInfo> fut = prom.get_future();
// 	std::unique_ptr<ExpertRequest> req = std::unique_ptr<ExpertRequest>(new ExpertRequest);
// 	req->robot = robot;
// 	req->prom = std::move(prom);
// 	erq_lock.lock();
// 	erq.push(std::move(req));
// 	erq_cv.notify_one();
// 	erq_lock.unlock();
// 	robot = fut.get();
// 	return robot;
// }

void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
	int engineer_id = id;
	int robot_type;
	CustomerRequest request;
	RobotInfo robot;

	ServerStub stub;

	stub.Init(std::move(socket));

	while (true) {
		request = stub.ReceiveRequest();
		if (!request.IsValid() || request.GetRequestType() <= 0 || request.GetRequestType() > 2) {
			break;	
		}

		if(request.GetRequestType() == 1){
			robot = CreateRegularRobot(request, engineer_id);
			stub.ShipRobot(robot);
		}
		else if(request.GetRequestType() == 2){ // Read customer record
    			CustomerRecord record;
    			auto it = customer_record.find(request.GetCustomerId());
    			if(it != customer_record.end()){
        				record.SetRecord(it->first, it->second);
    			} 			
				else {
        				record.SetRecord(request.GetCustomerId(), -1);
    			}
    			stub.ReturnRecord(record);
}
// 			 ClientStub.Order should take a customer request and return robot information.
// • ClientStub.ReadRecord should take a customer request and return a customer record.
// • ServerStub.ReceiveOrder should be renamed to ServerStub.ReceiveRequest and return cus-
// tomer requests.
// • ServerStub.ShipRobot should take robot information and send the robot information.
// • ServerStub.ReturnRecord should take a customer record and send the customer record.
		}
			
		// robot_type = order.GetRobotType();
		// switch (robot_type) {
		// 	case 0:
		// 		break;
		// 	case 1:
		// 		robot = CreateSpecialRobot(order, engineer_id);
		// 		break;
		// 	default:
		// 		std::cout << "Undefined robot type: "
		// 			<< robot_type << std::endl;

		// }
		stub.ShipRobot(robot);
	}


void RobotFactory::AdminThread(int id) {
	std::unique_lock<std::mutex> ul(erq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (erq.empty()) {
			erq_cv.wait(ul, [this]{ return !erq.empty(); });
		}

		auto req = std::move(erq.front());
		erq.pop();

		ul.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(100));
		req->robot.SetAdminId(id);
		req->prom.set_value(req->robot);	
	}
}



