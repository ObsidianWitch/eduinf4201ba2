/* T. Grandpierre - Application distribuée pour TP IF4-DIST 2004-2005

But :

fournir un squelette d'application capable de recevoir des messages en
mode non bloquant provenant de sites connus. L'objectif est de fournir
une base pour implementer les horloges logique/vectorielle/scalaire, ou
bien pour implementer l'algorithme d'exclusion mutuelle distribué

Syntaxe :
    arg 1 : Numero du 1er port
	arg 2 et suivant : nom de chaque machine

--------------------------------
Exemple pour 3 site :

Dans 3 shells lances sur 3 machines executer la meme application:

pc5201a>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr
pc5201b>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr
pc5201c>./dist 5000 pc5201a.esiee.fr pc5201b.esiee.fr pc5201c.esiee.fr

pc5201a commence par attendre que les autres applications (sur autres
sites) soient lancés

Chaque autre site (pc5201b, pc5201c) attend que le 1er site de la
liste (pc5201a) envoi un message indiquant que tous les sites sont lancés


Chaque Site passe ensuite en attente de connexion non bloquante (connect)
sur son port d'ecoute (respectivement 5000, 5001, 5002).
On fournit ensuite un exemple permettant
1) d'accepter la connexion
2) lire le message envoyé sur cette socket
3) il est alors possible de renvoyer un message a l'envoyeur ou autre si
necessaire
*/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include "host_tools.h"
#include "socket_tools.h"
#include "message.h"
#include "message_linked_list.h"

#define STATE_NOTHING 0
#define STATE_CRITICAL_SECTION 1
#define STATE_WAITING 2

int max(int a, int b);
void main_loop(int s_listen, int nhosts, char *argv[]);

int max(int a, int b) {
    return (a > b) ? a : b;
}

int main(int argc, char* argv[]) {
    int s_listen;
    int nhosts, base_port;

    if (argc < 3) {
        printf(
		    "Missing arguments (at least 2 hosts must be given)\n"
		    "Usage : %s base_port host1 host2 ...\n",
		    argv[0]
        );
        exit(EXIT_FAILURE);
    }

    nhosts = argc - 2;
    base_port = atoi(argv[1]) + get_host_pos(nhosts, argv);
    printf("Current host port: %d\n", base_port);

	s_listen = init_stream_server_socket(base_port);

    sync_hosts(s_listen, nhosts, argv);

    // Set the socket to non-blocking
    fcntl(s_listen, F_SETFL, O_NONBLOCK);

    main_loop(s_listen, nhosts, argv);

    close(s_listen);
    return EXIT_SUCCESS;
}

void main_loop(int s_listen, int nhosts, char *argv[]) {
    int responses = 0, logical_clock = 0;
    int state = STATE_NOTHING;
    int cur_host_id = get_host_pos(nhosts, argv);
    node* queue = NULL;

    while (1) {
        // -- Message retrieval & handling --
        int s_client = accept(s_listen, NULL, NULL);
        if (s_client > 0) {
            message *msg = receive_message(s_client);
            close(s_client);
            if (msg == NULL) {
                exit(EXIT_FAILURE);
            }

            logical_clock = 1 + max(logical_clock, msg->timestamp);

            printf("Host(%d) - Clock(%d) - Received (from %d) : %s\n",
                cur_host_id, logical_clock, msg->host_id, msg->str
            );

            if (strcmp(msg->str, "request") == 0) {
                message* msg_response;
                int recipient = msg->host_id;

                insert_message(&queue, msg);
                logical_clock++;
                print_messages_linked_list(queue); // FIXME DEBUG ONLY

                msg_response = create_message(
                    cur_host_id, logical_clock, "response"
                );

                send_message(argv[2 + recipient], atoi(argv[1]) + recipient,
                    msg_response);
                printf("host(%d) - Clock(%d) - Sent response (to %d)\n",
                    cur_host_id, logical_clock, recipient);
                free_message(msg_response);
            }
            else if (strcmp(msg->str, "response") == 0) {
                responses++;
                free_message(msg);
            }
            else if (strcmp(msg->str, "free") == 0) {
                pop(&queue);
                print_messages_linked_list(queue); // FIXME DEBUG ONLY
                free_message(msg);
            }
        }

        // -- State update --
        if (state == STATE_NOTHING) {
            // Choose randomly whether the current host must change state
            if ((rand() % nhosts) == cur_host_id) {
                message *request_msg;

                logical_clock++;

                request_msg = create_message(cur_host_id, logical_clock, "request");
                insert_message(&queue, request_msg);

                send_message_all(nhosts, cur_host_id, argv, request_msg);

                printf("host(%d) - Clock(%d) - Sent request\n",
                    cur_host_id, logical_clock);
                print_messages_linked_list(queue); // FIXME DEBUG ONLY

                state = STATE_WAITING;
            }
        }
        else if (state == STATE_WAITING &&
            queue->msg->host_id == cur_host_id &&
            responses == nhosts - 1)
        {
            responses = 0;
            state = STATE_CRITICAL_SECTION;
            logical_clock++;

            printf("Host(%d) - Clock(%d) - Begin critical section\n",
                cur_host_id, logical_clock);
        }

        if (state == STATE_CRITICAL_SECTION) {
            message *free_msg;

            logical_clock++;

            free_msg = create_message(cur_host_id, logical_clock, "free");
            send_message_all(nhosts, cur_host_id, argv, free_msg);
            free_message(free_msg);

            pop(&queue);

            printf("Host(%d) - Clock(%d) - End critical section\n",
                cur_host_id, logical_clock);
            print_messages_linked_list(queue); // FIXME DEBUG ONLY

            state = STATE_NOTHING;
        }
    }

    free_linked_list(&queue);
}
