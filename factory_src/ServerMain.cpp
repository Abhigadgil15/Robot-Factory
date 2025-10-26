#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include "ServerSocket.h"
#include "ServerThread.h"
#include <tuple>

struct PeerInfo {
    int id;
    std::string ip;
    int port;
};

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cout << "Not enough arguments - Usage: " 
                  << argv[0] << " [port] [unique_id] [num_peers] "
                  << "(repeat [ID] [IP] [port] for each peer)" << std::endl;
        return 0;
    }

    int port = atoi(argv[1]);
    int unique_id = atoi(argv[2]);
    int num_peers = atoi(argv[3]);

    if (argc != 4 + num_peers * 3) {
        std::cout << "Invalid number of arguments for peers." << std::endl;
        return 0;
    }

    // Parse peer info
    std::vector<std::tuple<int, std::string, int>> peers;
    for (int i = 0; i < num_peers; i++) {
        int idx = 4 + i * 3;
        int peer_id = atoi(argv[idx]);
        std::string peer_ip = argv[idx + 1];
        int peer_port = atoi(argv[idx + 2]);
        peers.push_back(std::make_tuple(peer_id, peer_ip, peer_port));
    }
	RobotFactory::SetFactoryId(unique_id);
    RobotFactory::SetPeerInfo(peers);

    ServerSocket socket;
    int engineer_cnt = 0;

    // Initialize server socket
    if (!socket.Init(port)) {
        std::cout << "Socket initialization failed" << std::endl;
        return 0;
    }

    std::cout << "Server " << unique_id << " running on port " << port 
              << " with " << num_peers << " peers." << std::endl;

    // Launch admin thread (1 per factory) - static method, no factory instance needed
    std::thread admin_thread(&RobotFactory::AdminThread, 0);
    admin_thread.detach();

    // Accept incoming engineer connections
    while (true) {
        std::unique_ptr<ServerSocket> new_socket = socket.Accept();
        if (!new_socket) {
            continue;
        }
        
        std::cout << "Server " << unique_id << " - Engineer " << engineer_cnt << " connected" << std::endl;
        
        std::thread engineer_thread(&RobotFactory::EngineerThread, 
                                    std::move(new_socket), engineer_cnt++);
        engineer_thread.detach();
    }

    return 0;
}