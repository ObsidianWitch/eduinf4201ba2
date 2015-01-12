#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct message {
	int host_id;
	int timestamp;
	char* str;
} message;

message* create_message(int host_id, int timestamp, const char* str);
void free_message(message* msg);

int send_message(char *hostname, int port, message* msg);
int send_message_all(int nhosts, int cur_host_id, char *argv[], message* msg);
char* pack_message(message* msg);

message* receive_message(int sockfd);
message* unpack_message(char* msg);

#endif // MESSAGE_H
