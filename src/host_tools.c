#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "host_tools.h"
#include "socket_tools.h"

/**
 * Retrieve the current host's id.
 *
 * @param nhosts Number of hosts
 * @param argv Arguments given to the program, contain the hostnames
 * @return current host's id
 */
int get_host_pos(int nhosts, char *argv[]) {
    char hostname[20];
    int hostpos, i;

    gethostname(hostname, 20);

    for (i = 0 ; i < nhosts ; i++) {
        if (strcmp(hostname, argv[i+2]) == 0) {
            hostpos = i;
            return hostpos;
        }
    }

    printf("Could not find hostname's index\n");
    exit(EXIT_FAILURE);

    return -1;
}

/**
 * Synchronize hosts between themselves. The first host waits for the sync
 * message from the other hosts, and then send them a sync message.
 *
 * @param s_listen Listening socket file descriptor
 * @param nhosts Number of hosts
 * @param argv Arguments given to the program, contain the hostnames
 */
void sync_hosts(int s_listen, int nhosts, char* argv[]) {
    int i;

    if (get_host_pos(nhosts, argv) == 0) {
        for (i = 0 ; i < nhosts - 1 ; i++) {
            wait_sync(s_listen);
        }

        printf("All Syncs received\n");fflush(0);

        for ( i = 0 ; i < nhosts - 1 ; i++) {
            send_sync(argv[3+i], atoi(argv[1]) + i + 1);
        }
    }
    else {
        send_sync(argv[2], atoi(argv[1]));

        printf("Wait sync from host 0\n"); fflush(0);
        wait_sync(s_listen);
        printf("Sync received from host 0\n"); fflush(0);
    }
}

/**
 * Wait (blocking) for an incoming synchronization message from the given socket
 * and print it.
 *
 * @param s_listen
 */
void wait_sync(int s_listen) {
    char* buf;
    int s_service;

    printf("wait_sync : "); fflush(0);

    s_service = accept(s_listen, NULL, NULL);
    buf = recv_complete(s_service);

    printf("%s\n",buf); fflush(0);
    close(s_service);
    free(buf);
}

/**
 * Send a synchronization message to the specified host and port.
 *
 * @param host
 * @param port
 */
void send_sync(char *host, int port) {
    char chaine[] = "**SYNCHRO**";
	send_complete_host(host, port, chaine, strlen(chaine));
}
