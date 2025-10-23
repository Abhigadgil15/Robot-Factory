#include "ServerStub.h"

ServerStub::ServerStub() {}

void ServerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

CustomerRequest ServerStub::ReceiveRequest() {
	char buffer[32];
	CustomerRequest request;
	if (socket->Recv(buffer, request.Size(), 0)) {
		request.Unmarshal(buffer);
	}
	return request;	
}

RobotInfo ServerStub::ShipRobot(RobotInfo info) {
	char buffer[32];
	if (socket->Recv(buffer, info.Size(), 0)) {
		info.Unmarshal(buffer);
	}
	return info;
}

CustomerRecord ServerStub::ReturnRecord(CustomerRecord record) {
	char buffer[32];
	if (socket->Recv(buffer, record.Size(), 0)) {
		record.Unmarshal(buffer);
	}
	return record;
}


