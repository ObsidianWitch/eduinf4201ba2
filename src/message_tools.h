#ifndef MESSAGE_TOOLS_H
#define MESSAGE_TOOLS_H

typedef struct message {
	int host_id;
	int timestamp;
	char* str;
} message;

message* create_message(int host_id, int timestamp, const char* str);
int send_message_complete(char *hostname, int port, message* msg);
char* pack_message(message* msg);
message* receive_message_complete(int sockfd);
message* unpack_message(char* msg);
void free_message(message* msg);

#endif // MESSAGE_TOOLS_H
