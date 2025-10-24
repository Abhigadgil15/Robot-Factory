#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <map>
#include "ServerSocket.h"
#include "ServerThread.h"

int main(int argc, char *argv[]) {
	int port;
	int engineer_cnt = 0;
	ServerSocket socket;
	RobotFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;
	
	if (argc < 2) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port #]" << std::endl;
		return 0;
	}
	port = atoi(argv[1]);
	// for (int i = 0; i < num_experts; i++) {
	// 	std::thread expert_thread(&RobotFactory::ExpertThread, &factory, engineer_cnt++);
	// 	thread_vector.push_back(std::move(expert_thread));
	// }



    if (!socket.Init(port)) {
        std::cout << "Socket initialization failed" << std::endl;
        return 0;
    }

	std::thread admin_thread(&RobotFactory::AdminThread, &factory, 0);
    thread_vector.push_back(std::move(admin_thread));

	while ((new_socket = socket.Accept())) {
		std::thread engineer_thread(&RobotFactory::EngineerThread, &factory, 
				std::move(new_socket), engineer_cnt++);
		thread_vector.push_back(std::move(engineer_thread));
	}

	for (auto &t : thread_vector) {
        if (t.joinable())
            t.join();
    }
	return 0;
}
