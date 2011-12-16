
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <xlocale.h>
#include <pthread.h>
#include <signal.h>

#include "fonctionsServeur.h"

#define CLEAN()	free(client); \
				free(thread_id); \
				free(socketClients); \
				close(sd);

#define W_GRILLE 6
#define H_GRILLE 4
#define FREQ_RAF 2

extern int h_errno;
extern int errno;
int sd, nbThreads = 0;
struct sockaddr_in* client;
pthread_t *thread_id = NULL;
int *socketClients = NULL;
int arret = 0;
pthread_mutex_t mutexThreads;


/* ending handler. Necessary to clean the environnement. */
void sigintHandler(int sig)
{
	printf("SIGINT received…\n");
	arret = 1;
}

void sigpipeHandler(int sig)
{
	printf("SIGPIPE received in thread %lu\n", (unsigned long)pthread_self());
}

void* gestionClient(void* arg)
{
	pthread_detach(pthread_self());
	
	/* Gestion des signaux */
	/*struct sigaction action;
	
	memset(&action, 0, sizeof(action));
	action.sa_handler = endthread;
	if (sigaction(SIGINT,&action,NULL))
	{
		perror("sigaction");
	}*/
	
	int i, numThread = -1, tourne = 1;
	int sd_client = *((int*)arg);
	char grille[W_GRILLE*H_GRILLE+H_GRILLE+1];
	for(i = 0 ; i < W_GRILLE*H_GRILLE+H_GRILLE ; i++)
	{
		if((i+1) % (W_GRILLE+1) == 0)
			grille[i] = '\n';
		else
			grille[i] = '0';
	}
	grille[1] = '1';
	
	printf("Client %d connecte sur le thread %lu avec la socket %d\n", nbThreads, (unsigned long)(pthread_self()), sd_client);
	while(tourne)
	{
		if(send(sd_client, grille, W_GRILLE*H_GRILLE+H_GRILLE+1, 0) < 1)
		{
			perror("Erreur d'envoi de la grille ");
			tourne = 0;
		}
		else
		{
			sleep(FREQ_RAF);
		}
	}
	
	pthread_mutex_lock(&mutexThreads);
	printf("Client %d deconnecte sur le thread %lu avec la socket %d\n", nbThreads, (unsigned long)(pthread_self()), sd_client);
	
	close(sd_client);
	for(i = 0 ; i < nbThreads-1 ; i++)
	{
		if(thread_id[i] == pthread_self())
		{
			numThread = i;
		}
		if(numThread != -1)
		{
			socketClients[i] = socketClients[i+1];
			thread_id[i] = thread_id[i+1];
		}
	}
	nbThreads--;
	thread_id = realloc(thread_id, nbThreads*sizeof(pthread_t));
	
	pthread_mutex_unlock(&mutexThreads);
	
	return NULL;
}

int main(int argc, char* argv[])
{
	struct hostent *hote = NULL;
	struct sockaddr_in server;
	int addr_in_size;
	int i = 1;
	int sd_client;
	pthread_t *thread_id_trans = NULL;
	int *socketClients_trans = NULL;
	
	/* Gestion des signaux */
	struct sigaction action;
	
	memset(&action, 0, sizeof(action));
	action.sa_handler = sigintHandler;
	if (sigaction(SIGINT,&action,NULL))
	{
		perror("sigaction");
	}
	action.sa_handler = sigpipeHandler;
	if (sigaction(SIGPIPE,&action,NULL))
	{
		perror("sigaction");
	}
	
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
		CLEAN()
		exit(EXIT_FAILURE);
	}
	printf("Adresse IP du serveur : %s\nPort du serveur : %d\n\n", (char*)inet_ntoa(server.sin_addr), htons(server.sin_port));
	
	if(bind(sd, (struct sockaddr*)&server, (socklen_t)sizeof(server)) == -1)
	{
		perror("Erreur de lien a la boite reseau ");
		CLEAN()
		exit(EXIT_FAILURE);
	}
	
	if(listen(sd, 10) == -1)
	{
		perror("listen");
		CLEAN()
		exit(EXIT_FAILURE);
	}
	
	
	printf("Serveur initialise sur le thread %lu\n", (unsigned long)pthread_self());
	
	
	while(!arret)
	{
		// accepter la connexion entrante
		if((sd_client = accept(sd, (struct sockaddr *)client, (socklen_t*)(&addr_in_size))) == -1)
		{
			perror("accept");
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
							   
	CLEAN()
	printf("Terminaison du serveur.\n");
	
	return 0;
}


