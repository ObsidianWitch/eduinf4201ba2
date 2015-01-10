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

int GetSitePos(int Nbsites, char *argv[]) ;
void WaitSync(int socket);
void send_sync(char *site, int Port);

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
 * Attente bloquante d'un msg de synchro sur la socket donnée
 * TODO refacto
 */
void WaitSync(int s_ecoute) {
    char texte[40];
    int l;
    int s_service;
    struct sockaddr_in sock_add_dist;
    socklen_t size_sock;

    size_sock = sizeof(struct sockaddr_in);
    printf("WaitSync : "); fflush(0);
    s_service = accept(s_ecoute,(struct sockaddr*) &sock_add_dist,&size_sock);
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

int main (int argc, char* argv[]) {
    struct sockaddr_in sock_add_dist;
    socklen_t size_sock;
    int s_ecoute, s_service;
    char texte[40];
    int i, l;
    float t;

    int base_port = -1; /*Numero du port de la socket a` creer*/
    int NSites = -1; /*Nb total de sites*/

    if (argc < 3) {
        printf(
		    "Missing arguments (at least 2 hosts must be given)\n"
		    "Usage : %s base_port host1 host2 ...\n",
		    argv[0]
        );
        exit(EXIT_FAILURE);
    }

    /*----Nombre de sites (adresses de machines)---- */
    NSites = argc - 2;

    /*CREATION&BINDING DE LA SOCKET DE CE SITE*/
    base_port = atoi(argv[1]) + GetSitePos(NSites, argv);
    printf("Numero de port de ce site %d\n", base_port);

	s_ecoute = init_stream_server_socket(base_port);

    if (GetSitePos(NSites, argv) == 0) {
        /*Le site 0 attend une connexion de chaque site : */
        for (i = 0 ; i < NSites - 1 ; i++) {
            WaitSync(s_ecoute);
        }

        printf("Toutes les synchros recues \n");fflush(0);

        /*et envoie un msg a chaque autre site pour les synchroniser */
        for ( i = 0 ; i < NSites - 1 ; i++) {
            send_sync(argv[3+i], atoi(argv[1]) + i + 1);
        }
    }
    else {
        /* Chaque autre site envoie un message au site0
        (1er  dans la liste) pour dire qu'il est lance'*/
        send_sync(argv[2], atoi(argv[1]));
        /*et attend un message du Site 0 envoye' quand tous seront lance's*/
        printf("Wait Synchro du Site 0\n"); fflush(0);
        WaitSync(s_ecoute);
        printf("Synchro recue de Site 0\n"); fflush(0);
    }

    /* Passage en mode non bloquant du accept pour tous*/
    /*---------------------------------------*/
    fcntl(s_ecoute,F_SETFL,O_NONBLOCK);
    size_sock = sizeof(struct sockaddr_in);

    /* Boucle infini*/
    while(1) {
        /* On commence par tester l'arrivée d'un message */
        s_service = accept(s_ecoute,(struct sockaddr*) &sock_add_dist, &size_sock);
        if (s_service > 0) {
            /*Extraction et affichage du message */
            l = read(s_service, texte, 39);
            texte[l] = '\0';
            printf("Message recu : %s\n", texte); fflush(0);
            close(s_service);
        }

        /* Petite boucle d'attente : c'est ici que l'on peut faire des choses*/
        for (l = 0 ; l < 1000000 ; l++) {
            t = t*3;
            t = t/3;
        }

        printf("."); fflush(0); /* pour montrer que le serveur est actif*/
    }

    close(s_ecoute);
    return EXIT_SUCCESS;
}
