#include "ClientStub.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);	
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

