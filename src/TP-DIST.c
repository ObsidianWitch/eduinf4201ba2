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
#include "socket_tools.h"
#include "message_tools.h"
#include "message_linked_list.h"

int GetSitePos(int Nbsites, char *argv[]) ;
void sync_hosts(int s_listen, int nhosts, char* argv[]);
void WaitSync(int socket);
void send_sync(char *site, int Port);
void main_loop_body(int s_listen);

/**
 * Identification de ma position dans la liste
 */
int GetSitePos(int NbSites, char *argv[]) {
    char MySiteName[20];
    int MySitePos = -1;
    int i;
    gethostname(MySiteName, 20);

    for (i = 0 ; i < NbSites ; i++) {
        if (strcmp(MySiteName, argv[i+2]) == 0) {
            MySitePos = i;
            //printf("L'indice de %s est %d\n",MySiteName,MySitePos);
            return MySitePos;
        }
    }

    if (MySitePos == -1) {
        printf("Indice du Site courant non trouve' dans la liste\n");
        exit(EXIT_FAILURE);
    }

    return -1;
}

/**
 * Synchronize hosts between themselves. The first host waits for the sync message from the other
 * hosts, and then send them a sync message.
 *
 * @param s_listen Listening socket file descriptor
 * @param nhosts Number of hosts
 * @param argv Arguments given to the program, contain the hostnames
 */
void sync_hosts(int s_listen, int nhosts, char* argv[]) {
    int i;

    if (GetSitePos(nhosts, argv) == 0) {
        for (i = 0 ; i < nhosts - 1 ; i++) {
            WaitSync(s_listen);
        }

        printf("Toutes les synchros recues \n");fflush(0);

        for ( i = 0 ; i < nhosts - 1 ; i++) {
            send_sync(argv[3+i], atoi(argv[1]) + i + 1);
        }
    }
    else {
        send_sync(argv[2], atoi(argv[1]));

        printf("Wait Synchro du Site 0\n"); fflush(0);
        WaitSync(s_listen);
        printf("Synchro recue de Site 0\n"); fflush(0);
    }
}

/**
 * Attente bloquante d'un msg de synchro sur la socket donnée
 * TODO refacto
 */
void WaitSync(int s_listen) {
    char texte[40];
    int l;
    int s_service;
    struct sockaddr_in sock_add_dist;
    socklen_t size_sock;

    size_sock = sizeof(struct sockaddr_in);
    printf("WaitSync : "); fflush(0);
    s_service = accept(s_listen,(struct sockaddr*) &sock_add_dist,&size_sock);
    l = read(s_service,texte,39);
    texte[l] = '\0';
    printf("%s\n",texte); fflush(0);
    close(s_service);
}

/**
 * Send a synchronization message to the specified host:port
 */
void send_sync(char *site, int port) {
    char chaine[15];

    sprintf(chaine,"**SYNCHRO**");
	send_complete_host(site, port, chaine, strlen(chaine));
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
    base_port = atoi(argv[1]) + GetSitePos(nhosts, argv);
    printf("Numero de port de ce site %d\n", base_port);

	s_listen = init_stream_server_socket(base_port);

    sync_hosts(s_listen, nhosts, argv);

    // Set the socket to non-blocking
    fcntl(s_listen, F_SETFL, O_NONBLOCK);

    while(1) {
        main_loop_body(s_listen);
    }

    close(s_listen);
    return EXIT_SUCCESS;
}

void main_loop_body(int s_listen) {
    int s_client;
    char* buf;

    s_client = accept(s_listen, NULL, NULL);
    if (s_client > 0) {
        buf = recv_complete(s_client);
        printf("Message recu : %s\n", buf); fflush(0);
        close(s_client);
    }

    // FIXME B Function

    // FIXME E Function
}
