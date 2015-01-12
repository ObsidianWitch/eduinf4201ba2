#ifndef HOST_TOOLS_H
#define HOST_TOOLS_H

int get_host_pos(int nhosts, char *argv[]) ;
void sync_hosts(int s_listen, int nhosts, char* argv[]);
void wait_sync(int socket);
void send_sync(char *host, int Port);

#endif // HOST_TOOLS_H
