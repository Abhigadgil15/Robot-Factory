#include "ServerStub.h"
#include <cstring>

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
    
    info.Marshal(buffer);
    socket->Send(buffer, info.Size(), 0);
    
    return info;
}

CustomerRecord ServerStub::ReturnRecord(CustomerRecord record) {
    char buffer[32];
    
    record.Marshal(buffer);
    socket->Send(buffer, record.Size(), 0);
    
    return record;
}