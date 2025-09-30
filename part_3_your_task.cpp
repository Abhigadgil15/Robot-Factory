#include <iostream>
#include <string>

#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include "Message.h"

/******************************************************************************
    Part 3 - 1
******************************************************************************/

// This is a function that the client program will use
// before sending the request to the server.
// The argument, buffer, is a C-string or a character array
// that needs to be filled-in in this function body.
// TODO: return value should match with the amount of
// data encoded into the buffer.
int Request::Marshal(char *buffer)
{
    // int offset = 0;
    // memset(buffer, 0, 64);

    // TODO: Write a code that marshals req_id, user_id
    //  requester (32 character string), num1, and
    //  num2 into a C-string (i.e., buffer) for the
    //  transfer over the network. Note that (unsigned) int and short
    //  types need to be encoded into network format
    //  using hton functions (use htons or htonl depending on
    //  the size of data). Requester only needs to be converted
    //  to a C-string and does not need to be network encoded,
    //  but make sure the string is a null terminating
    //  C-string (i.e., ends with '\0' character) .
    //  The format should be
    //  [req_id 4B][user_id 2B][requester 32B][num1 4B][num2 4B].
    //  You will need to use memcpy to copy the data to the buffer.
    int offset = 0;

    // clear the buffer if you like
    // memset(buffer, 0, 64);

    uint32_t net_req_id = htonl(req_id);
    uint16_t net_user_id = htons(user_id);
    uint32_t net_num1 = htonl(num1);
    uint32_t net_num2 = htonl(num2);

    memcpy(buffer + offset, &net_req_id, sizeof(net_req_id));
    offset += sizeof(net_req_id);

    memcpy(buffer + offset, &net_user_id, sizeof(net_user_id));
    offset += sizeof(net_user_id);

    char name[32] = {0};
    strncpy(name, requester.c_str(), 31);
    memcpy(buffer + offset, name, 32);
    offset += 32;

    memcpy(buffer + offset, &net_num1, sizeof(net_num1));
    offset += sizeof(net_num1);

    memcpy(buffer + offset, &net_num2, sizeof(net_num2));
    offset += sizeof(net_num2);

    return offset;
}

// This is a function that the server program will use
// after receiving the request from the client to
// parse the Message content out of the C-string.
// The argument, buffer, is a C-string or a character array
// which contains the marshalled data.
void Request::Unmarshal(char *buffer)
{
    // TODO: unmarshal input C-string (buffer) and assign the
    // retrieved values to the member variables of this class.
    // Input buffer format should be
    // [req_id 4B][user_id 2B][requester 32B][num1 4B][num2 4B].
    // (unsigned) int and short types are in network format so you will need to
    // convert them to host format using ntoh functions (use ntons or ntonl
    // depending on the size of data). Requester only needs to be converted
    // to std::string and does not need to be converted to the host format.

    int offset = 0;

    uint32_t net_req_id;
    uint16_t net_user_id;
    uint32_t net_num1;
    uint32_t net_num2;
    char name[33] = {0};

    memcpy(&net_req_id, buffer + offset, sizeof(net_req_id));
    offset += sizeof(net_req_id);

    memcpy(&net_user_id, buffer + offset, sizeof(net_user_id));
    offset += sizeof(net_user_id);

    memcpy(name, buffer + offset, 32);
    offset += 32;

    memcpy(&net_num1, buffer + offset, sizeof(net_num1));
    offset += sizeof(net_num1);

    memcpy(&net_num2, buffer + offset, sizeof(net_num2));
    offset += sizeof(net_num2);

    req_id = ntohl(net_req_id);
    user_id = ntohs(net_user_id);
    requester = std::string(name);
    num1 = ntohl(net_num1);
    num2 = ntohl(net_num2);
}

// This is a function that the server program will use
// before sending the response to the client.
// The argument, buffer, is a C-string or a character array
// that needs to be filled-in in this function body.
// TODO: return value should match with the amount of
// data encoded into the buffer.
int Response::Marshal(char *buffer)
{
    // TODO: Write a code that marshals req_id, user_id,
    //  and sum into a C-string (i.e., buffer) for the
    //  transfer over the network. Note that (unsigned) int and short
    //  types need to be encoded into network format
    //  using hton variant functions (use htons or htonl depending on
    //  the size of data).
    //  The format should be
    //  [req_id 4B][user_id 2B][res_id 4B][response 4B].

    // feel free to change "return 0;" below.
    int offset = 0;

    uint32_t net_req_id = htonl(req_id);
    uint16_t net_user_id = htons(user_id);
    uint32_t net_res_id = htonl(res_id);
    uint32_t net_sum = htonl(sum);

    memcpy(buffer + offset, &net_req_id, sizeof(net_req_id));
    offset += sizeof(net_req_id);

    memcpy(buffer + offset, &net_user_id, sizeof(net_user_id));
    offset += sizeof(net_user_id);

    memcpy(buffer + offset, &net_res_id, sizeof(net_res_id));
    offset += sizeof(net_res_id);

    memcpy(buffer + offset, &net_sum, sizeof(net_sum));
    offset += sizeof(net_sum);

    return offset;
}

// This is a function that the client program will use
// after receiving the response from the server.
// Parse the Message content out of the C-string.
// The argument, buffer, is a C-string or a character array
// which contains the marshalled data.
void Response::Unmarshal(char *buffer)
{
    // TODO: unmarshal input C-string (buffer) and assign the
    // retrieved values to the member variables of this class.
    // Input buffer format should be
    // [req_id 4B][user_id 2B][res_id 4B][response 4B].
    // (unsigned) int and short types are in network format so you will need to
    // convert them to host format using ntoh functions (use ntons or ntonl
    // depending on the size of data).

    int offset = 0;
    uint32_t net_req_id, net_res_id, net_sum;
    uint16_t net_user_id;

    memcpy(&net_req_id, buffer + offset, sizeof(net_req_id));
    offset += sizeof(net_req_id);

    memcpy(&net_user_id, buffer + offset, sizeof(net_user_id));
    offset += sizeof(net_user_id);

    memcpy(&net_res_id, buffer + offset, sizeof(net_res_id));
    offset += sizeof(net_res_id);

    memcpy(&net_sum, buffer + offset, sizeof(net_sum));
    offset += sizeof(net_sum);

    req_id = ntohl(net_req_id);
    user_id = ntohs(net_user_id);
    res_id = ntohl(net_res_id);
    sum = ntohl(net_sum);
}

/******************************************************************************
    Part 3 - 2
******************************************************************************/

// This is the code going into the client of Part 3 - 2
void part_3_client_marshal_send_recv_unmarshal(Request &req, Response &res,
                                               char *buffer, int sockfd)
{
    int req_size = req.Marshal(buffer);

    if (send(sockfd, buffer, req_size, 0) != req_size)
    {
        perror("client send");
        return;
    }

    int n = recv(sockfd, buffer, 64, 0);
    if (n <= 0)
    {
        perror("client recv");
        return;
    }

    res.Unmarshal(buffer);

    // TODO:
    // 1) get marshalled byte array (i.e., buffer) using Request (i.e., req).
    // 2) send the marshalled buffer over the network.
    // 3) wait and receive server's response.
    // 4) unmarshal the received byte array into Response (i.e., res).
}

// This is the code going into the server of Part 3 - 2
void part_3_server_recv_unmarshal(Request &req, char *buffer, int sockfd)
{
    int n = recv(sockfd, buffer, 64, 0);
    if (n <= 0)
    {
        perror("server recv");
        return;
    }

    req.Unmarshal(buffer);

    // TODO:
    // 1) receive the request in byte array using the socket from the client
    // 2) unmashal the received byte array into Request (i.e., res)
}

// This is the code going into the server of Part 3 - 2
void part_3_server_marshal_send(Response &res, char *buffer, int sockfd)
{

    int res_size = res.Marshal(buffer);

    if (send(sockfd, buffer, res_size, 0) != res_size)
    {
        perror("server send");
    }
    // TODO:
    // 1) get marshalled byte array (i.e., buffer) using Response (i.e., res).
    // 2) send the marshalled buffer over the network using send.
}




