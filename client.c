#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "gui.h"
#include "pers_sock.h"
#include "fonctionsClient.h"


#define GRID_W 48
#define GRID_H 32
#define CLEAN()	if(close(sd) == -1){perror("Erreur de close");}

int arret = 0;

void fn_thread (void *WINDOW);

void endloop () 
{
	arret = 1;
}


int main(int argc, char **argv) {
	p_host_t hote = NULL;
	sockin_t server, client;
	int grid_size = GRID_H * (GRID_W + 1) + 1;
	char recvit[grid_size];
	int nbLus, sd;

	/* Déclaration des variables de NCurse */
	int startx, starty, width, height, ch, i;
	win_t **allwin = init_screen();

	pthread_t thread[1];
	
	bzero(&server,sizeof(server));
	bzero(&client,sizeof(client));

	/* Lecture des options */
	read_options(argc, argv, server, client);

	/* Gestion des signaux */
	struct sigaction action;
	
	memset(&action, 0, sizeof(action));
	action.sa_handler = endloop;
	
	if (sigaction(SIGINT,&action,NULL)){
		perror ("sigaction");
		exit(EXIT_FAILURE);
	}
	
	/* Connexion au serveur */
	sd = create_socket_by_structure(client, NAMED_SOCK);
	
	if(connect(sd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		perror("Erreur de connection ");
		CLEAN()
		exit(EXIT_FAILURE);
	}
	
	printf("Client connecte.\n");
	
	while(!arret)
	{
		nbLus = recv(sd, recvit, grid_size, 0);
		if(nbLus < 1)
		{
			if(nbLus == 0)
				fprintf(stderr, "Serveur deconnecte\n");
			else
				perror("Erreur de reception ");
			arret = 1;
		}
		print_window (allwin[DISP_WIN], recvit, 1, 2);
	}
	
	CLEAN()

	printf("Fermeture du client.\n");
	
	return EXIT_SUCCESS;
}


