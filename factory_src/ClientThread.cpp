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

    if (request_type == 1) {
        for (int i = 0; i < num_requests; i++) {
            CustomerRequest request;
            request.SetRequest(customer_id, i, 1);

            timer.Start();
            RobotInfo robot = stub.Order(request);
            timer.EndAndMerge();

            if (!robot.IsValid()) {
                std::cout << "Invalid robot " << customer_id << std::endl;
                break;
            }
        }
    }
    else if (request_type == 2) {
        // Read own record
        CustomerRequest request;
        request.SetRequest(customer_id, -1, 2);

        timer.Start();
        CustomerRecord record = stub.ReadRecord(request);  // Use ReadRecord!
        timer.EndAndMerge();

        if (record.IsValid() && record.GetCustomerId() != -1) {
            std::cout << "Customer Record:" << std::endl;
            std::cout << "  Customer ID: " << record.GetCustomerId() << std::endl;
            std::cout << "  Last Order: " << record.GetLastOrder() << std::endl;
        } else {
            std::cout << "Customer record not found for ID " << customer_id << std::endl;
        }
    }
    else if (request_type == 3) {
        // Scan all records
        std::cout << "Customer ID\tLast Order" << std::endl;
        for (int cid = 0; cid <= num_requests; cid++) {
            CustomerRequest request;
            request.SetRequest(cid, -1, 2);

            timer.Start();
            CustomerRecord record = stub.ReadRecord(request);  // Use ReadRecord!
            timer.EndAndMerge();

            if (record.IsValid() && record.GetCustomerId() != -1) {
                std::cout << cid << "\t\t" << record.GetLastOrder() << std::endl;
            }
        }
    }
}

ClientTimer ClientThreadClass::GetTimer() {
    return timer;
}