#ifndef SOCKET_TOOLS_H
#define SOCKET_TOOLS_H

#include <netinet/in.h>

int init_stream_server_socket(int port);
int init_stream_client_socket(const char* hostname, int port);
int send_complete(int sockfd, char* msg, int msg_size);
int send_complete_host(char *hostname, int port, char* msg, int msg_size);
char* recv_complete(int sockfd);

#endif // SOCKET_TOOLS_H
