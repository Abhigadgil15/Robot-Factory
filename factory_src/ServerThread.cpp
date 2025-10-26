#include <iostream>
#include <memory>
#include "ServerThread.h"
#include "ServerStub.h"
#include "FactorySocket.h"
#include <map>
#include <vector>
#include "Messages.h"

// Initialize ALL static members
std::vector<MapOp> RobotFactory::smr_log;
std::map<int, int> RobotFactory::customer_record;
std::queue<CustomerRequest> RobotFactory::pending_requests;

int RobotFactory::last_index = -1;
int RobotFactory::committed_index = -1;
int RobotFactory::primary_id = -1;
int RobotFactory::factory_id = -1;

// Initialize peer connection static members
std::vector<std::tuple<int, std::string, int>> RobotFactory::peer_factories;
std::vector<std::unique_ptr<ClientSocket>> RobotFactory::backup_connections;

std::mutex RobotFactory::queue_mutex;
std::mutex RobotFactory::map_mutex;
std::condition_variable RobotFactory::cv_admin;
std::condition_variable RobotFactory::cv_engineer;

void RobotFactory::SetFactoryId(int id) {
    factory_id = id;
    std::cout << "[RobotFactory] Factory ID set to " << id << std::endl;
}

void RobotFactory::SetPeerInfo(const std::vector<std::tuple<int, std::string, int>>& peers) {
    peer_factories = peers;
    for (const auto& peer : peers) {
        std::cout << "[RobotFactory] Added peer " << std::get<0>(peer) 
                  << ": " << std::get<1>(peer) << ":" << std::get<2>(peer) << std::endl;
    }
}

RobotInfo RobotFactory::CreateRegularRobot(CustomerRequest request, int engineer_id) {
    RobotInfo robot;
    robot.CopyRequest(request);
    robot.SetEngineerId(engineer_id);
    robot.SetAdminId(-1);
    return robot;
}

void RobotFactory::ApplyOperation(int index) {
    if (index < 0 || index >= static_cast<int>(smr_log.size())) {
        return;
    }
    
    MapOp op = smr_log[index];
    if (op.opcode == 1) {
        customer_record[op.arg1] = op.arg2;
        std::cout << "[ApplyOperation] Applied log[" << index << "]: customer " 
                  << op.arg1 << " = " << op.arg2 << std::endl;
    }
}

void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
    int engineer_id = id;
    
    // Step 1: Receive identification message FIRST (before moving socket)
    IdentificationRequest id_msg;
    char id_buffer[16];
    
    if (socket->Recv(id_buffer, id_msg.Size(), 0) <= 0) {
        std::cerr << "[EngineerThread " << engineer_id << "] Failed to receive ID" << std::endl;
        return;
    }
    
    id_msg.Unmarshal(id_buffer);
    int connection_type = id_msg.GetType();
    
    std::cout << "[EngineerThread " << engineer_id << "] Connection type: " 
              << (connection_type == 0 ? "Customer" : "PFA") << std::endl;
    
    // Step 2: NOW decide what to do based on type
    if (connection_type == 0) {
        // ROLE: Engineer (handle customer requests)
        HandleCustomerRequests(std::move(socket), engineer_id);
    } else if (connection_type == 1) {
        // ROLE: IFA (handle replication)
        HandleReplicationRequests(std::move(socket), engineer_id);
    }
}

void RobotFactory::HandleCustomerRequests(std::unique_ptr<ServerSocket> socket, int engineer_id) {
    CustomerRequest request;
    ServerStub stub;
    stub.Init(std::move(socket));  // NOW move socket to stub

    while (true) {
        request = stub.ReceiveRequest();
        if (!request.IsValid() || request.GetRequestType() <= 0 || request.GetRequestType() > 2) {
            break;
        }

        if (request.GetRequestType() == 1) {
            RobotInfo robot = CreateRegularRobot(request, engineer_id);

            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                pending_requests.push(request);
            }
            cv_admin.notify_one();

            {
                std::unique_lock<std::mutex> lock(map_mutex);
                cv_engineer.wait(lock, [&]() {
                    auto it = customer_record.find(request.GetCustomerId());
                    return it != customer_record.end() && 
                           it->second == request.GetOrderNumber();
                });
            }

            stub.ShipRobot(robot);
        }
        else if (request.GetRequestType() == 2) { 
            CustomerRecord record;
            {
                std::unique_lock<std::mutex> lock(map_mutex);
                auto it = customer_record.find(request.GetCustomerId());
                if (it != customer_record.end()) {
                    record.SetRecord(it->first, it->second);
                } else {
                    record.SetRecord(-1, -1);
                }
            }
            stub.ReturnRecord(record);
        }
    }
}

void RobotFactory::HandleReplicationRequests(std::unique_ptr<ServerSocket> socket, int thread_id) {
    std::cout << "[IFA " << thread_id << "] Started handling replication" << std::endl;
    
    while (true) {
        ReplicationRequest req;
        char buffer[256];
        
        if (socket->Recv(buffer, req.Size(), 0) <= 0) {
            std::cout << "[IFA " << thread_id << "] Connection closed" << std::endl;
            break;
        }
        
        req.Unmarshal(buffer);
        std::cout << "[IFA " << thread_id << "] Received replication from primary " 
                  << req.GetFactoryId() << std::endl;
        
        {
            std::unique_lock<std::mutex> lock(map_mutex);
            
            if (primary_id != req.GetFactoryId()) {
                primary_id = req.GetFactoryId();
                std::cout << "[IFA " << thread_id << "] Set primary_id to " << primary_id << std::endl;
            }
            
            int idx = req.GetLastIndex();
            if (idx >= static_cast<int>(smr_log.size())) {
                smr_log.resize(idx + 1);
            }
            smr_log[idx] = req.GetOperation();
            last_index = idx;
            std::cout << "[IFA " << thread_id << "] Wrote to log[" << last_index << "]" << std::endl;
            
            while (committed_index < req.GetCommittedIndex()) {
                committed_index++;
                ApplyOperation(committed_index);
            }
        }
        
        ReplicationResponse resp;
        resp.SetAck(1);
        char ack_buffer[16];
        resp.Marshal(ack_buffer);
        socket->Send(ack_buffer, resp.Size(), 0);
        
        std::cout << "[IFA " << thread_id << "] Sent ack to primary" << std::endl;
    }
}

void RobotFactory::AdminThread(int admin_id) {
    while (true) {
        CustomerRequest request;

        {
            std::unique_lock<std::mutex> ul(queue_mutex);
            cv_admin.wait(ul, []() { 
                return !pending_requests.empty(); 
            });
            request = pending_requests.front();
            pending_requests.pop();
        }

        // Check if we need to become primary
        bool need_to_connect = false;
        {
            std::unique_lock<std::mutex> lock(map_mutex);
            if (primary_id != factory_id) {
                primary_id = factory_id;
                need_to_connect = true;
            }
        }

        // Establish connections OUTSIDE the lock
        if (need_to_connect) {
            std::cout << "[AdminThread] Becoming primary (id=" << factory_id << ")" << std::endl;
            
            backup_connections.clear();
            for (const auto& peer : peer_factories) {
                std::unique_ptr<ClientSocket> socket(new ClientSocket());
                
                std::cout << "[Primary] Attempting to connect to backup " << std::get<0>(peer) 
                          << " at " << std::get<1>(peer) << ":" << std::get<2>(peer) << std::endl;
                
                if (socket->Init(std::get<1>(peer), std::get<2>(peer))) {
                    // Send identification message
                    IdentificationRequest id_msg;
                    id_msg.SetType(1);  // 1 = PFA
                    char id_buffer[16];
                    id_msg.Marshal(id_buffer);
                    
                    if (socket->Send(id_buffer, id_msg.Size(), 0) > 0) {
                        backup_connections.push_back(std::move(socket));
                        std::cout << "[Primary] Successfully connected to backup " << std::get<0>(peer) << std::endl;
                    } else {
                        std::cout << "[Primary] Failed to send ID to backup " << std::get<0>(peer) << std::endl;
                    }
                } else {
                    std::cout << "[Primary] Failed to connect to backup " << std::get<0>(peer) << std::endl;
                }
            }
            
            std::cout << "[Primary] Total backup connections: " << backup_connections.size() << std::endl;
        }

        // Create operation
        MapOp op;
        op.opcode = 1;
        op.arg1 = request.GetCustomerId();
        op.arg2 = request.GetOrderNumber();

        // Append to log
        {
            std::unique_lock<std::mutex> lock(map_mutex);
            smr_log.push_back(op);
            last_index++;
            std::cout << "[Primary] Appended to log[" << last_index << "]" << std::endl;
        }

        // Replicate to all backups
        int successful = 0;
        std::cout << "[Primary] Replicating to " << backup_connections.size() << " backups" << std::endl;
        
        for (size_t i = 0; i < backup_connections.size(); i++) {
            ReplicationRequest rep_req;
            {
                std::unique_lock<std::mutex> lock(map_mutex);
                rep_req.SetRequest(factory_id, committed_index, last_index, op);
            }

            char buffer[256];
            rep_req.Marshal(buffer);
            
            std::cout << "[Primary] Sending replication to backup " << i << std::endl;
            
            if (backup_connections[i]->Send(buffer, rep_req.Size(), 0) > 0) {
                ReplicationResponse resp;
                char ack_buffer[16];
                if (backup_connections[i]->Recv(ack_buffer, resp.Size(), 0) > 0) {
                    resp.Unmarshal(ack_buffer);
                    if (resp.GetAck() == 1) {
                        successful++;
                        std::cout << "[Primary] Received ack from backup " << i << std::endl;
                    }
                }
            } else {
                std::cout << "[Primary] Failed to send to backup " << i << std::endl;
            }
        }

        // Commit if all backups responded
        std::cout << "[Primary] Successful replications: " << successful 
                  << " / " << backup_connections.size() << std::endl;
                  
        if (successful == static_cast<int>(backup_connections.size())) {
            std::unique_lock<std::mutex> lock(map_mutex);
            ApplyOperation(last_index);
            committed_index = last_index;
            std::cout << "[Primary] Committed log[" << committed_index << "]" << std::endl;
        } else {
            std::cout << "[Primary] WARNING: Not all backups acknowledged!" << std::endl;
        }

        cv_engineer.notify_all();
    }

}