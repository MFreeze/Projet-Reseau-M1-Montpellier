

#include "fonctionsServeurR.h"

#define FULL_TIME_CONTROL 60


char* grille;
int arret = 0;
int camMoving = 0;
int nbMouvements = 0;
int *socketClients = NULL;
pthread_mutex_t mutexSockets;
int nbClients = 0;

/* Thread principal du serveur de reception */
int main(int argc, char* argv[])
{
	struct sockaddr_in server;
	int addr_in_size = sizeof(struct sockaddr_in);
	int i = 1;
	int sd_client;
	pthread_t thread_id = -1;
	int *socketClients_trans = NULL;
	struct sockaddr_in* client;
	int sd, joined;
	time_t timeBthread, timeCur;
	int timeControl = FULL_TIME_CONTROL;
	
	client = (struct sockaddr_in *)malloc(addr_in_size);
	bzero(client,sizeof(client));
	
	/* Gestion des signaux */
	attachSignals();
	
	/* Initialisation du mutex gerant l'acces au tableau de sockets client */
	pthread_mutex_init(&mutexSockets, NULL);
	
	/* Recuperation du segment de memoire partagee de la grille */
	grille = gridRecupAddr(argv[1]);
	
	/* Init reseau */
	if((sd = gstArgs(argv, &server)) < 1)
	{
		free(client);
		kill(getppid(), SIGINT);
		if(sd == 0)
			exit(EXIT_SUCCESS);
		else
			exit(EXIT_FAILURE);
	}
	
	/* Ecoute des demandes de connexion des clients */
	if(listen(sd, 1) == -1)
	{
		perror("listen");
		free(client);
		close(sd);
		kill(getppid(), SIGINT);
		exit(EXIT_FAILURE);
	}

	printf ("Serveur de reception initialise.\n\n");
	
	/* boucle d'execution : acceptation des clients et creation du thread */
	while(!arret)
	{
		// accepter la connexion entrante
		if(nbClients < 19 && (sd_client = accept(sd, (struct sockaddr *)client, (socklen_t*)(&addr_in_size))) != -1)
		{
			pthread_mutex_lock(&mutexSockets);
			if(socketClients == NULL) {
				socketClients = malloc(sizeof(int));
				setNonblocking(sd);
			}
			else
			{
				socketClients_trans = malloc((nbClients+1)*sizeof(int));
				for(i = 0 ; i < nbClients ; i++)
				{
					socketClients_trans[i] = socketClients[i];
				}
				free(socketClients);
				socketClients = socketClients_trans;
				socketClients_trans = NULL;
			}
			socketClients[nbClients] = sd_client;
			nbClients++;
			pthread_mutex_unlock(&mutexSockets);
			timeControl = FULL_TIME_CONTROL / nbClients;
			printf("Client sur la socket %d en liste d'attente pour le controle.\n", sd_client);
		}
		if(camMoving == 1)
		{
			timeCur = time(NULL);
			if(timeCur - timeBthread >= timeControl)
			{
				printf("Le temps du client (%ds) sur la socket %d est ecoule.\n", timeControl, socketClients[0]);
				close(socketClients[0]);
				pthread_join(thread_id, NULL);
				joined = 1;
				timeControl = nbClients > 0 ? (FULL_TIME_CONTROL / nbClients) : 60;
			}
			else
			{
				usleep(10000);
			}
		}
		if(camMoving == 0 && thread_id != -1 && !joined)
		{
			pthread_join(thread_id, NULL);
			joined = 1;
			timeControl = nbClients > 0 ? (FULL_TIME_CONTROL / nbClients) : 60;
		}
		if(camMoving == 0 && nbClients > 0)
		{
			timeBthread = time(NULL);
			
			/* lancement de l'execution du thread qui s'occupera du client */
			pthread_mutex_lock(&mutexSockets);
			if(pthread_create(&thread_id, NULL, thread_deplacement, NULL) != 0)
			{
				fprintf(stderr, "Thread creation failure.\n");
				arret = 1;
			}
			pthread_mutex_unlock(&mutexSockets);
			camMoving = 1;
			joined = 0;
		}
		if(nbClients == 0)
		{
			setBlocking(sd);
		}
	}
	
	
	/* clean */
	for(i = 0 ; i < nbClients ; i++)
	{
		printf("Close socket %d\n", socketClients[i]);
		if(close(socketClients[i]) < 0)
		{
			perror("erreur close ");
		}
	}
	
	
	free(client);
	free(socketClients);
	close(sd);
	if(shmdt(grille) == -1)
	{
		perror("Shared memory segment's detachment impossible");
	}
	
	printf("Terminaison du serveur de reception.\n");
	
	return 0;
}


