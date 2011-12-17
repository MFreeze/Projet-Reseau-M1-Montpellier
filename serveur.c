

#include "fonctionsServeur.h"

int *socketClients = NULL;
int nbThreads = 0;
pthread_t *thread_id = NULL;
pthread_mutex_t mutexThreads;
int grilleShm;
char* grille;

int arret = 0;



/* Main server thread */
int main(int argc, char* argv[])
{
	struct hostent *hote = NULL;
	struct sockaddr_in server;
	int addr_in_size;
	int i = 1;
	int sd_client;
	pthread_t *thread_id_trans = NULL;
	int *socketClients_trans = NULL;
	struct sockaddr_in* client;
	int sd;
	
	/* Gestion des signaux */
	attachSignals();
	
	grille = gridCreation(argv[0], &grilleShm, W_GRILLE, H_GRILLE);
	
	/* init mutex */
	pthread_mutex_init(&mutexThreads, NULL);
	
	/* init reseau */
	bzero(&server,sizeof(server));
	
	gstArgs(argc, argv, hote, &server);
	
	addr_in_size = sizeof(struct sockaddr_in);
	client = (struct sockaddr_in *)malloc(addr_in_size);
	bzero(client,sizeof(client));
	
	
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd<0)
	{
		perror("Erreur d'ouverture de socket ");
		free(hote);
		free(client);
		exit(EXIT_FAILURE);
	}
	printf("Adresse IP du serveur : %s\nPort du serveur : %d\n\n", (char*)inet_ntoa(server.sin_addr), htons(server.sin_port));
	
	if(bind(sd, (struct sockaddr*)&server, (socklen_t)sizeof(server)) == -1)
	{
		perror("Erreur de lien a la boite reseau ");
		free(hote);
		free(client);
		close(sd);
		exit(EXIT_FAILURE);
	}
	
	if(listen(sd, 10) == -1)
	{
		perror("listen");
		free(hote);
		free(client);
		close(sd);
		exit(EXIT_FAILURE);
	}
	
	
	printf("Serveur initialise sur le thread %lu\n", (unsigned long)pthread_self());
	
	
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
		
		if(pthread_create(&(thread_id[nbThreads]), NULL, gestionClient, &sd_client) != 0)
		{
			fprintf(stderr, "Thread %lu creation failure.\n", (unsigned long)(thread_id));
			arret = 1;
		}
		nbThreads++;
		pthread_mutex_unlock(&mutexThreads);
	}
	
	
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
	
	free(hote);
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
	
	printf("Terminaison du serveur.\n");
	
	return 0;
}


