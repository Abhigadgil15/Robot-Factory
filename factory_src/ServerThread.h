#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <map>
#include <vector>
#include <tuple>
#include <memory>

#include "Messages.h"
#include "ServerSocket.h"
#include "FactorySocket.h"


class RobotFactory
{
private:
    static std::vector<MapOp> smr_log;
    static std::map<int, int> customer_record;
    static std::queue<CustomerRequest> pending_requests;
    
    static int last_index;
    static int committed_index;
    static int primary_id;
    static int factory_id;
    
    // Peer connection info
    static std::vector<std::tuple<int, std::string, int>> peer_factories;
    static std::vector<std::unique_ptr<ClientSocket>> backup_connections;
    
    static std::mutex queue_mutex;
    static std::mutex map_mutex;
    static std::condition_variable cv_admin;
    static std::condition_variable cv_engineer;

    static RobotInfo CreateRegularRobot(CustomerRequest request, int engineer_id);
    static void ApplyOperation(int index);

public:
    static void SetFactoryId(int id);
    static void SetPeerInfo(const std::vector<std::tuple<int, std::string, int>>& peers);
    static void HandleReplicationRequests(std::unique_ptr<ServerSocket> socket, int thread_id);
	static void HandleCustomerRequests(std::unique_ptr<ServerSocket> socket, int engineer_id);
    static void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
    static void AdminThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__