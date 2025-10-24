#include <array>
#include <iostream> 
#include <iomanip> 
#include <thread> 
#include <vector> 

#include "ClientSocket.h"
#include "ClientThread.h"
#include "ClientTimer.h"

int main(int argc, char *argv[]) {
	std::string ip;
	int port;
	int num_customers;
	int num_orders;
	int request_type;
	ClientTimer timer;

	std::vector<std::shared_ptr<ClientThreadClass>> client_vector;
	std::vector<std::thread> thread_vector;
	
	if (argc < 6) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << " [ip] [port #] [# customers] [# requests] [request type 1 or 2 or 3]" << std::endl;
		return 0;
	}

	ip = argv[1];
	port = atoi(argv[2]);
	num_customers = atoi(argv[3]);
	num_orders = atoi(argv[4]);
	request_type = atoi(argv[5]);

	// ✅ Validate request type
	if (request_type < 1 || request_type > 3) {
		std::cerr << "Error: Invalid request type. Must be 1, 2, or 3." << std::endl;
		return 1;
	}

	timer.Start();
	for (int i = 0; i < num_customers; i++) {
		auto client_cls = std::make_shared<ClientThreadClass>();
		std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
				ip, port, i, num_orders, request_type);

		client_vector.push_back(client_cls);
		thread_vector.push_back(std::move(client_thread));
	}

	for (auto &th : thread_vector) {
		th.join();
	}

	timer.End();

	for (auto &cls : client_vector) {
		timer.Merge(cls->GetTimer());
	}

	timer.PrintStats();

	return 0;
}
