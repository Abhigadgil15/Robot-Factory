#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include "Messages.h"
#include "ServerSocket.h"

struct AdminRequest
{
	RobotInfo robot;
	std::promise<RobotInfo> prom;
};

class RobotFactory
{
private:
	std::map<int, int> customer_record;			  // shared map
	std::queue<CustomerRequest> pending_requests; // requests waiting for admin
	std::mutex queue_mutex;						  // protects map and queue
	std::condition_variable cv_admin;			  // admin waits on this
	std::condition_variable cv_engineer;
	std::vector<MapOp> smr_log;

	RobotInfo CreateRegularRobot(CustomerRequest request, int engineer_id);

public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void AdminThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__
