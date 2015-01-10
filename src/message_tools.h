#ifndef MESSAGE_TOOLS_H
#define MESSAGE_TOOLS_H

typedef struct message {
	int el;
	char* str;
} message;


int send_message_complete(char *hostname, int port, message* msg);
char* pack_message(message* msg);
message* receive_message_complete(int sockfd);
message* unpack_message(char* msg);

#endif // MESSAGE_TOOLS_H
