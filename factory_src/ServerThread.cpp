#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"
#include <map>

RobotInfo RobotFactory::CreateRegularRobot(CustomerRequest request, int engineer_id)
{
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

void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id)
{
	int engineer_id = id;
	CustomerRequest request;
	RobotInfo robot;

	ServerStub stub;

	stub.Init(std::move(socket));

	while (true)
	{
		request = stub.ReceiveRequest();
		if (!request.IsValid() || request.GetRequestType() <= 0 || request.GetRequestType() > 2)
		{
			break;
		}

		if (request.GetRequestType() == 1)
		{
			// Step 1: Create robot
			RobotInfo robot = CreateRegularRobot(request, engineer_id);

			// Step 2: Push to pending_requests for admin
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				pending_requests.push(request);
				cv_admin.notify_one(); // notify admin
			}

			// Step 3: Wait until admin updates customer_record
            {
                std::unique_lock<std::mutex> lock(map_mutex);
                cv_engineer.wait(lock, [&]() {
                    auto it = customer_record.find(request.GetCustomerId());
                    return it != customer_record.end() && 
                           it->second == request.GetOrderNumber();
                });
            }

			// Step 4: Ship robot
			stub.ShipRobot(robot);
		}
		else if (request.GetRequestType() == 2)
		{ // Read customer record
			CustomerRecord record;
			{
				std::unique_lock<std::mutex> lock(map_mutex);
				auto it = this->customer_record.find(request.GetCustomerId());
				if (it != customer_record.end())
				{
					record.SetRecord(it->first, it->second);
				}
				else
				{
					record.SetRecord(request.GetCustomerId(), -1);
				}
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
}

// void RobotFactory::AdminThread(int id) {
//     std::unique_lock<std::mutex> ul(queue_mutex, std::defer_lock);
// 	while (true) {
// 		ul.lock();
// 		if (erq.empty()) {
// 			erq_cv.wait(ul, [this]{ return !erq.empty(); });
// 		}
// 		auto req = std::move(erq.front());
// 		erq.pop();
// 		ul.unlock();
// 		std::this_thread::sleep_for(std::chrono::microseconds(100));
// 		req->robot.SetAdminId(id);
// 		req->prom.set_value(req->robot);
// 	}
// }

void RobotFactory::AdminThread(int admin_id)
{
	while (true)
	{
		CustomerRequest request;

		// Get the next request
		{
			std::unique_lock<std::mutex> ul(queue_mutex);
			cv_admin.wait(ul, [this]
						  { return !pending_requests.empty(); });
			request = pending_requests.front();
			pending_requests.pop();
		}
		// Update map safely
		{
			std::unique_lock<std::mutex> lock(map_mutex);
			customer_record[request.GetCustomerId()] = request.GetOrderNumber();
			smr_log.push_back({1, request.GetCustomerId(), request.GetOrderNumber()});
		}

		// Notify engineer (if using CV)
		cv_engineer.notify_all();
	}
}
