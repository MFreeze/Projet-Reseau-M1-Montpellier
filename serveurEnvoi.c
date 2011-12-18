

#include "fonctionsServeurE.h"

int *socketClients = NULL;
int nbThreads = 0;
pthread_t *thread_id = NULL;
pthread_mutex_t mutexThreads;
int grilleShm;
char* grille;

int arret = 0;



/* Thread principal du serveur d'envoi */
int main(int argc, char* argv[])
{
	struct sockaddr_in server;
	int addr_in_size = sizeof(struct sockaddr_in);
	int i = 1, portSec;
	pthread_t *thread_id_trans = NULL;
	int *socketClients_trans = NULL;
	struct sockaddr_in* client;
	int sd, sd_client;
	pid_t servRecept;
	
	client = (struct sockaddr_in *)malloc(addr_in_size);
	bzero(client,sizeof(client));
	
	/* Gestion des signaux */
	attachSignals();
	
	/* Creation du segment de memoire partagee de la grille */
	grille = gridCreation(argv[0], &grilleShm);
	
	/* Initialisation du mutex gerant l'acces aux tableaux de threads et de sockets client */
	pthread_mutex_init(&mutexThreads, NULL);
	
	/* init reseau */
	if((sd = gstArgs(argc, argv, &server, 13321, &portSec)) < 1)
	{
		free(client);
		if(shmdt(grille) == -1)
		{
			perror("Shared memory segment's detachment impossible");
		}
		if(shmctl(grilleShm, IPC_RMID, NULL) == -1)
		{
			perror("Shared memory segment's destruction impossible");
		}
		
		if(sd == 0)
			exit(EXIT_SUCCESS);
		else
			exit(EXIT_FAILURE);
	}
	
	/* lancement du serveur de reception */
	servRecept = fork();
	if(servRecept == -1)
	{
		free(client);
		close(sd);
		if(shmdt(grille) == -1)
		{
			perror("Shared memory segment's detachment impossible");
		}
		if(shmctl(grilleShm, IPC_RMID, NULL) == -1)
		{
			perror("Shared memory segment's destruction impossible");
		}
	}
	else if(servRecept == 0)
	{
		execl("./serveurRecept", "serveurRecept", argv[0], itoa(server.sin_addr.s_addr), itoa(portSec), (char *)0);
	}
	
	//printf("Serveur initialise sur le thread %lu\n", (unsigned long)pthread_self());
	
	/* Ecoute des demandes de connexion des clients */
	if(listen(sd, 10) == -1)
	{
		perror("listen");
		free(client);
		close(sd);
		exit(EXIT_FAILURE);
	}
	
	/* boucle d'execution : acceptation des clients et creation des threads associes */
	while(!arret)
	{
		// accepter la connexion entrante
		if((sd_client = accept(sd, (struct sockaddr *)client, (socklen_t*)(&addr_in_size))) == -1)
		{
			//perror("accept");
			continue;
		}
		pthread_mutex_lock(&mutexThreads);
		// creer une nouvelle socket
		if(socketClients == NULL)
			socketClients = malloc(sizeof(int));
		else
		{
			socketClients_trans = malloc((nbThreads+1)*sizeof(int));
			for(i = 0 ; i < nbThreads ; i++)
			{
				socketClients_trans[i] = socketClients[i];
			}
			free(socketClients);
			socketClients = socketClients_trans;
			socketClients_trans = NULL;
		}
		socketClients[nbThreads] = sd_client;
		// creer un nouveau thread
		if(thread_id == NULL)
			thread_id = malloc(sizeof(pthread_t));
		else
		{
			thread_id_trans = malloc((nbThreads+1)*sizeof(pthread_t));
			for(i = 0 ; i < nbThreads ; i++)
			{
				thread_id_trans[i] = thread_id[i];
			}
			free(thread_id);
			thread_id = thread_id_trans;
			thread_id_trans = NULL;
		}
		/* lancement de l'execution du thread qui s'occupera du client nouvellement connecte */
		if(pthread_create(&(thread_id[nbThreads]), NULL, thread_broadcast, &sd_client) != 0)
		{
			fprintf(stderr, "Thread %lu creation failure.\n", (unsigned long)(thread_id));
			arret = 1;
		}
		nbThreads++;
		pthread_mutex_unlock(&mutexThreads);
	}
	
	
	/* clean */
	pthread_mutex_lock(&mutexThreads);
	for(i = 0 ; i < nbThreads ; i++)
	{
		printf("Close socket %d\n", socketClients[i]);
		if(close(socketClients[i]) < 0)
		{
			perror("erreur close ");
		}
		pthread_kill(thread_id[i], SIGINT);
	}
	pthread_mutex_unlock(&mutexThreads);
	
	
	free(client);
	free(thread_id);
	free(socketClients);
	close(sd);
	if(shmdt(grille) == -1)
	{
		perror("Shared memory segment's detachment impossible");
	}
	if(shmctl(grilleShm, IPC_RMID, NULL) == -1)
	{
		perror("Shared memory segment's destruction impossible");
	}
	
	printf("Terminaison du serveur d'envoi.\n");
	
	return 0;
}


