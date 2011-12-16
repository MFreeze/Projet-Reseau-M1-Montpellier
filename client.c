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

#include "fonctionsClient.h"


#define GRID_W 48
#define GRID_H 32
#define CLEAN()	if(close(sd) == -1){perror("Erreur de close");}

int arret = 0;

void endloop () 
{
	arret = 1;
}


int main(int argc, char* argv[])
{
	struct hostent *hote = NULL;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int grid_size = GRID_H * (GRID_W + 1) + 1;
	char recvit[grid_size];
	int nbLus;
	int sd;
	
	bzero(&server,sizeof(server));
	bzero(&client,sizeof(client));
	
	gstArgs(argc, argv, hote, &server, &client);
	
	/* Gestion des signaux */
	struct sigaction action;
	
	memset(&action, 0, sizeof(action));
	action.sa_handler = endloop;
	
	if (sigaction(SIGINT,&action,NULL)){
		perror ("sigaction");
		exit(EXIT_FAILURE);
	}
	
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		perror("Erreur d'ouverture de socket ");
		exit(EXIT_FAILURE);
	}
	printf("Port du client : %d\n", htons(client.sin_port));
	printf("Adresse IP du serveur : %s\n", (char*)inet_ntoa(server.sin_addr));
	printf("Port du serveur : %d\n\n", htons(server.sin_port));
	
	if(bind(sd, (struct sockaddr*)&client, sizeof(client)) == -1)
	{
		perror("Erreur de lien a la boite reseau ");
		CLEAN()
		exit(EXIT_FAILURE);
	}
	
	if(connect(sd, (struct sockaddr*)&server, sizeof(server)) == -1)
	{
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
		printf("%s\n\n", recvit);
	}
	
	CLEAN()
	printf("Fermeture du client.\n");
	
	return EXIT_SUCCESS;
}


