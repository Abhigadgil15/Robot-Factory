#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"




RobotOrder::RobotOrder() {
	customer_id = -1;
	order_number = -1;
	robot_type = -1;
}

void RobotOrder::SetOrder(int id, int number, int type) {
	customer_id = id;
	order_number = number;
	robot_type = type;
}

int RobotOrder::GetCustomerId() { return customer_id; }
int RobotOrder::GetOrderNumber() { return order_number; }
int RobotOrder::GetRobotType() { return robot_type; }

int RobotOrder::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(robot_type);
}

void RobotOrder::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_robot_type = htonl(robot_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
}

void RobotOrder::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_robot_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	robot_type = ntohl(net_robot_type);
}

bool RobotOrder::IsValid() {
	return (customer_id != -1);
}

void RobotOrder::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << robot_type << std::endl;
}

RobotInfo::RobotInfo() {
    customer_id = -1;
    order_number = -1;
    request_type = -1;  // changed
    engineer_id = -1;
    admin_id = -1;      // changed
}

void RobotInfo::SetInfo(int id, int number, int type, int engid, int admid) {
    customer_id = id;
    order_number = number;
    request_type = type;  // changed
    engineer_id = engid;
    admin_id = admid;     // changed
}

void RobotInfo::CopyRequest(CustomerRequest request) {
    customer_id = request.GetCustomerId();
    order_number = request.GetOrderNumber();
    request_type = request.GetRequestType();  
}

void RobotInfo::SetEngineerId(int id) { engineer_id = id; }
void RobotInfo::SetAdminId(int id) { admin_id = id; } // changed

int RobotInfo::GetCustomerId() { return customer_id; }
int RobotInfo::GetOrderNumber() { return order_number; }
int RobotInfo::GetRequestType() { return request_type; } // changed
int RobotInfo::GetEngineerId() { return engineer_id; }
int RobotInfo::GetAdminId() { return admin_id; }          // changed

int RobotInfo::Size() {
    return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type)
        + sizeof(engineer_id) + sizeof(admin_id);  // changed
}

void RobotInfo::Marshal(char *buffer) {
    int net_customer_id = htonl(customer_id);
    int net_order_number = htonl(order_number);
    int net_request_type = htonl(request_type);  // changed
    int net_engineer_id = htonl(engineer_id);
    int net_admin_id = htonl(admin_id);          // changed
    int offset = 0;

    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);
    memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
    offset += sizeof(net_order_number);
    memcpy(buffer + offset, &net_request_type, sizeof(net_request_type)); // changed
    offset += sizeof(net_request_type);
    memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
    offset += sizeof(net_engineer_id);
    memcpy(buffer + offset, &net_admin_id, sizeof(net_admin_id)); // changed
}

void RobotInfo::Unmarshal(char *buffer) {
    int net_customer_id;
    int net_order_number;
    int net_request_type; // changed
    int net_engineer_id;
    int net_admin_id;     // changed
    int offset = 0;

    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);
    memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
    offset += sizeof(net_order_number);
    memcpy(&net_request_type, buffer + offset, sizeof(net_request_type)); // changed
    offset += sizeof(net_request_type);
    memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id));
    offset += sizeof(net_engineer_id);
    memcpy(&net_admin_id, buffer + offset, sizeof(net_admin_id)); // changed

    customer_id = ntohl(net_customer_id);
    order_number = ntohl(net_order_number);
    request_type = ntohl(net_request_type); // changed
    engineer_id = ntohl(net_engineer_id);
    admin_id = ntohl(net_admin_id);         // changed
}

bool RobotInfo::IsValid() {
    return (customer_id != -1);
}

void RobotInfo::Print() {
    std::cout << "id " << customer_id << " ";
    std::cout << "num " << order_number << " ";
    std::cout << "type " << request_type << " "; // changed
    std::cout << "engid " << engineer_id << " ";
    std::cout << "adminid " << admin_id << std::endl; // changed
}




/// CustomerRequest class implementation

CustomerRequest::CustomerRequest() {
	customer_id = -1;
	order_number = -1;
	request_type = -1;
}
void CustomerRequest::SetRequest(int id, int number, int type) {
	customer_id = id;
	order_number = number;
	request_type = type;
}
int CustomerRequest::GetCustomerId() { return customer_id; }
int CustomerRequest::GetOrderNumber() { return order_number; }
int CustomerRequest::GetRequestType() { return request_type; }
int CustomerRequest::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type);
}

void CustomerRequest::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_request_type = htonl(request_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
}

void CustomerRequest::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_request_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_request_type);
}

bool CustomerRequest::IsValid() {
	return (customer_id != -1);
}

void CustomerRequest::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << request_type << std::endl;
}





// CustomerRecord class implementation
CustomerRecord::CustomerRecord() {
	customer_id = -1;
	last_order = -1;
}
void CustomerRecord::SetRecord(int cid, int order_num) {
	customer_id = cid;
	last_order = order_num;
}
int CustomerRecord::GetCustomerId() { return customer_id; }
int CustomerRecord::GetLastOrder() { return last_order; }
int CustomerRecord::Size() {
	return sizeof(customer_id) + sizeof(last_order);
}
void CustomerRecord::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_last_order = htonl(last_order);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_last_order, sizeof(net_last_order));
}
void CustomerRecord::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_last_order;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_last_order, buffer + offset, sizeof(net_last_order));

	customer_id = ntohl(net_customer_id);
	last_order = ntohl(net_last_order);
}
bool CustomerRecord::IsValid() {
	return (customer_id != -1);
}
void CustomerRecord::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "last_order " << last_order << std::endl;
}

ReplicationRequest::ReplicationRequest() 
    : factory_id(-1), committed_index(-1), last_index(-1) {
    op.opcode = 0;
    op.arg1 = 0;
    op.arg2 = 0;
}

void ReplicationRequest::SetRequest(int fid, int cindex, int lindex, const MapOp& operation) {
    factory_id = fid;
    committed_index = cindex;
    last_index = lindex;
    op = operation;
}

int ReplicationRequest::GetFactoryId() const {
    return factory_id;
}

int ReplicationRequest::GetCommittedIndex() const {
    return committed_index;
}

int ReplicationRequest::GetLastIndex() const {
    return last_index;
}

MapOp ReplicationRequest::GetOperation() const {
    return op;
}

int ReplicationRequest::Size() const {
    return sizeof(factory_id) + sizeof(committed_index) + sizeof(last_index) +
           sizeof(op.opcode) + sizeof(op.arg1) + sizeof(op.arg2);
}

void ReplicationRequest::Marshal(char *buffer) const {
    int offset = 0;
    memcpy(buffer + offset, &factory_id, sizeof(factory_id));
    offset += sizeof(factory_id);
    memcpy(buffer + offset, &committed_index, sizeof(committed_index));
    offset += sizeof(committed_index);
    memcpy(buffer + offset, &last_index, sizeof(last_index));
    offset += sizeof(last_index);
    memcpy(buffer + offset, &op.opcode, sizeof(op.opcode));
    offset += sizeof(op.opcode);
    memcpy(buffer + offset, &op.arg1, sizeof(op.arg1));
    offset += sizeof(op.arg1);
    memcpy(buffer + offset, &op.arg2, sizeof(op.arg2));
}

void ReplicationRequest::Unmarshal(char *buffer) {
    int offset = 0;
    memcpy(&factory_id, buffer + offset, sizeof(factory_id));
    offset += sizeof(factory_id);
    memcpy(&committed_index, buffer + offset, sizeof(committed_index));
    offset += sizeof(committed_index);
    memcpy(&last_index, buffer + offset, sizeof(last_index));
    offset += sizeof(last_index);
    memcpy(&op.opcode, buffer + offset, sizeof(op.opcode));
    offset += sizeof(op.opcode);
    memcpy(&op.arg1, buffer + offset, sizeof(op.arg1));
    offset += sizeof(op.arg1);
    memcpy(&op.arg2, buffer + offset, sizeof(op.arg2));
}

// ===== ReplicationResponse Implementation =====
ReplicationResponse::ReplicationResponse() : ack(0) {}

void ReplicationResponse::SetAck(int acknowledgment) {
    ack = acknowledgment;
}

int ReplicationResponse::GetAck() const {
    return ack;
}

int ReplicationResponse::Size() const {
    return sizeof(ack);
}

void ReplicationResponse::Marshal(char *buffer) const {
    memcpy(buffer, &ack, sizeof(ack));
}

void ReplicationResponse::Unmarshal(char *buffer) {
    memcpy(&ack, buffer, sizeof(ack));
}


IdentificationRequest::IdentificationRequest() : id_type(-1) {}

void IdentificationRequest::SetType(int type) {
    id_type = type;
}

int IdentificationRequest::GetType() const {
    return id_type;
}

int IdentificationRequest::Size() const {
    return sizeof(id_type);
}

void IdentificationRequest::Marshal(char *buffer) const {
    memcpy(buffer, &id_type, sizeof(id_type));
}

void IdentificationRequest::Unmarshal(char *buffer) {
    memcpy(&id_type, buffer, sizeof(id_type));
}