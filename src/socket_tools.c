#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "socket_tools.h"

#define BUFFER_LEN 1024

/**
 * Initializes a stream socket used as a server.
 * Exits the application if one of the steps to create and initialize the socket
 * fails.
 *
 * @param port on which the socket will be bound
 * @return Created socket's file descriptor.
 */
int init_stream_server_socket(int port) {
    int sockfd, status;
    struct sockaddr_in serv_addr;

    // Create socket (IPv4, connected, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("server - socket init");
        exit(EXIT_FAILURE);
    }

    // Fill serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

    // Bind socket to all local interfaces on the port specified in argument
    status = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (status == -1) {
        perror("server - bind");
        exit(EXIT_FAILURE);
    }

    // Passive (listening) socket
    status = listen(sockfd, 30);
    if (status == -1) {
        perror("server - listen");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * Initializes a stream socket used as a client.
 * Exits the application if one of the steps to create and initialize the socket
 * fails.
 *
 * @param hostname
 * @param port Host's port
 * @return Created socket's file descriptor.
 */
int init_stream_client_socket(const char* hostname, int port) {
    int sockfd, status;
    struct hostent *he;
    struct sockaddr_in dest_addr;

    // Retrieve server information
    he = gethostbyname(hostname);
    if (he == NULL) {
        perror("client - gethostbyname");
        exit(EXIT_FAILURE);
    }

    // Create socket (IPv4, connected, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("client - socket init");
        exit(EXIT_FAILURE);
    }

    // Fill dest_addr
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr = *((struct in_addr*) he->h_addr_list[0]);
    memset(dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero));

    // Connect the socket
    status = connect(sockfd, (struct sockaddr*) &dest_addr, sizeof(dest_addr));
    if (status == -1) {
        perror("client - connect");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * Send a complete message with the connected socket specified in parameter
 * (sockfd file descriptor).
 *
 * @param sockfd sending connected socket
 * @param msg message to send
 * @param msg_size size of the message to send
 * @return Returns 0 on success, and -1 if the message could not completely be
 * sent.
 */
int send_complete(int sockfd, char* msg, int msg_size) {
    int sent_size = 0;

    while (sent_size < msg_size) {
        int remaining_size = msg_size - sent_size;
        int temp_size = send(sockfd, msg + sent_size, remaining_size, 0);
        if (temp_size == -1) {
            perror("send");
            break;
        }

        sent_size += temp_size;
    }

    return (sent_size != msg_size) ? -1 : 0;
}

/**
 * Send a complete message to the specified host:port
 *
 * @param hostname
 * @param port
 */
int send_complete_host(char *hostname, int port, char* msg, int msg_size) {
    int s_emis, status;

    s_emis = init_stream_client_socket(hostname, port);
    status = send_complete(s_emis, msg, msg_size);

    close(s_emis);
    
    return status;
}

/**
 * Receive a complete message by making multiple recv calls, and each time
 * a recv call returns, prints it.
 *
 * @param sockfd
 * @return 0 on success, -1 otherwise.
*/
int recv_print(int sockfd) {
    int recv_size;
    char buf[BUFFER_LEN];

    do {
        recv_size = read(sockfd, buf, BUFFER_LEN - 1);
        if (recv_size == -1) {
            perror("read");
            break;
        }

        if (recv_size != 0) {
            buf[recv_size] = '\0';
            printf("%s", buf);
        }
    } while(recv_size != 0);

    return recv_size;
}

/**
 * Receive a complete message by making multiple recv calls.
 *
 * @param sockfd
 * @return message on success, NULL otherwise
*/
char* recv_complete(int sockfd) {
	int total_recv_size = 0;
    int recv_size;
    char *buf = malloc(BUFFER_LEN);

    do {
        recv_size = read(sockfd, buf + total_recv_size,
        	BUFFER_LEN - total_recv_size - 1);
        if (recv_size == -1) {
            perror("read");
            return NULL;
        }

        if (recv_size != 0) {
            buf[recv_size] = '\0';
            printf("%s", buf);
        }
        
        total_recv_size += recv_size;
    } while(recv_size != 0);

    return buf;
}
