#ifndef SOCKET_TOOLS_H
#define SOCKET_TOOLS_H

#include <netinet/in.h>

int init_stream_server_socket(int port);
int init_stream_client_socket(const char* hostname, int port);
int send_complete(int sockfd, char* msg, int msg_size);
int recv_print(int sockfd);
char* recv_complete(int sockfd);

#endif // SOCKET_TOOLS_H
