#include <iostream>
#include "ClientStub.h"
#include "Messages.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
    int result = socket.Init(ip, port);
    
    if (result) {
        IdentificationRequest id_msg;
        id_msg.SetType(0);  // 0 = customer
        char buffer[16];
        id_msg.Marshal(buffer);
        socket.Send(buffer, id_msg.Size(), 0);
        
        std::cout << "[ClientStub] Sent customer identification" << std::endl;
    }
    
    return result;
}

RobotInfo ClientStub::Order(CustomerRequest request) {
	RobotInfo info;
	char buffer[32];
	int size;
	request.Marshal(buffer);
	size = request.Size();
	if (socket.Send(buffer, size, 0)) {
		size = info.Size();
		if (socket.Recv(buffer, size, 0)) {
			info.Unmarshal(buffer);
		} 
	}
	return info;
}


CustomerRecord ClientStub::ReadRecord(CustomerRequest request) {
	CustomerRecord record;
	char buffer[32];
	int size;
	request.Marshal(buffer);
	size = request.Size();
	if (socket.Send(buffer, size, 0)) {
		size = record.Size();
		if (socket.Recv(buffer, size, 0)) {
			record.Unmarshal(buffer);
		} 
	}
	return record;
}

