#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message_tools.h"
#include "socket_tools.h"

#define BUFFER_LEN 256

/**
 * Send a complete message (defined by the message structure).
 *
 * @param sockfd sending connected socket
 * @param hostname
 * @param port
 * @param msg message to send
 * @return Returns 0 on success, and -1 otherwise.
 */
int send_message_complete(char *hostname, int port, message* msg) {
	int status;
	char* packed_message;

	packed_message = pack_message(msg);
	if (packed_message == NULL) {
		return -1;
	}

	status = send_complete_host(hostname, port, packed_message,
		strlen(packed_message));

	return status;
}

/**
 * Create a packed message (char*) from a message struct.
 *
 * @param msg message to pack
 * @return packed message
 */
char* pack_message(message* msg) {
	char* buf = malloc(BUFFER_LEN);

	int status = snprintf(buf, BUFFER_LEN,
		"%d - %s",
		msg->el,
		msg->str
	);

	if (status == 0) {
		free(buf);
		return NULL;
	}

	return buf;
}

/**
 * Receive a complete message (defined by the message structure).

 * @param sockfd
 * @return received message on success, NULL otherwise
 */
message* receive_message_complete(int sockfd) {
	char* recv_msg;
	message* unpacked_message;

	recv_msg = recv_complete(sockfd);
	if (recv_msg == NULL) {
		return NULL;
	}

	unpacked_message = unpack_message(recv_msg);

	return unpacked_message;
}

/**
 * Create a message structure from a string (char*).
 *
 * @param msg char* which will be parsed and transformed in a message struct
 * @return the unpacked message
 */
message* unpack_message(char* msg) {
	message* unpacked_message = malloc(sizeof(message));
	int el, status;
	char* buf = malloc(BUFFER_LEN);

	status = sscanf(msg, "%d - %s", &el, buf);
	if (status != 2) {
		return NULL;
	}

	unpacked_message->el  = el;
	unpacked_message->str = buf;

	return unpacked_message;
}
