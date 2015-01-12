#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message.h"
#include "socket_tools.h"

#define BUFFER_LEN 32

/**
 * Create a message from the given parameters.
 *
 * @param host_id message's host_id
 * @param timestamp message's timestamp
 * @param str message's string, will be copied
 * @return newly create message
 */
message* create_message(int host_id, int timestamp, const char* str) {
	message* msg = malloc(sizeof(message));

	msg->host_id = host_id;
	msg->timestamp = timestamp;
	msg->str = malloc(strlen(str) + 1);

	/* The string is copied to be sure we can free it later. Problems may occur
	 * otherwise if a string stored in the stack is given to a message. */
	strcpy(msg->str, str);

	return msg;
}

/**
 * Free a message and the string contained in this message.
 * @param msg Message to free
 */
void free_message(message* msg) {
	free(msg->str);
	free(msg);
}

/**
 * Send a message (defined by the message structure).
 *
 * @param sockfd sending connected socket
 * @param hostname
 * @param port
 * @param msg Message to send
 * @return Returns 0 on success, and -1 otherwise.
 */
int send_message(char *hostname, int port, message* msg) {
	int status;
	char* packed_message;

	packed_message = pack_message(msg);
	if (packed_message == NULL) {
		return -1;
	}

	status = send_complete_host(hostname, port, packed_message,
		strlen(packed_message));
	free(packed_message);

	return status;
}

/**
 * Send a message to all hosts (current host excluded).
 *
 * @param nhosts Number of hosts
 * @param cur_host_id Current host's id
 * @param argv Arguments given to the program, contain the hostnames
 * @param msg Message to send
 * @return 0 on success, -1 if one or more messages could not be sent
 * successfully.
 */
int send_message_all(int nhosts, int cur_host_id, char *argv[], message* msg) {
	int i, status = 0;

    for (i = 0 ; i < nhosts ; i++) {
        if (i != cur_host_id) {
            status += send_message(
				argv[2 + i], atoi(argv[1]) + i, msg
			);
        }
    }

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
		"%d - %d - %s",
		msg->host_id,
		msg->timestamp,
		msg->str
	);

	if (status == 0) {
		free(buf);
		return NULL;
	}

	return buf;
}

/**
 * Receive a message (defined by the message structure).

 * @param sockfd
 * @return received message on success, NULL otherwise
 */
message* receive_message(int sockfd) {
	char* recv_msg;
	message* unpacked_message;

	recv_msg = recv_complete(sockfd);
	if (recv_msg == NULL) {
		return NULL;
	}

	unpacked_message = unpack_message(recv_msg);
	free(recv_msg);

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
	int host_id, timestamp, status;
	char* buf = malloc(BUFFER_LEN);

	status = sscanf(msg, "%d - %d - %s", &host_id, &timestamp, buf);
	if (status != 3) {
		return NULL;
	}

	unpacked_message->host_id = host_id;
	unpacked_message->timestamp = timestamp;
	unpacked_message->str = buf;

	return unpacked_message;
}
