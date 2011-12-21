
#include "fonctionsServeurE.h"

int gstArgs(int argc, char* argv[], struct sockaddr_in *server, int portDefault, int* portSec)
{
	struct hostent *hote = NULL;
	bzero(server,sizeof(*server));
	struct in_addr adServer;
	bzero(&adServer,sizeof(adServer));
	int i = 1, sd = -1;
	
	if(argc == 2 && strcmp(argv[1], "--help")==0)
	{
		printf("Arguments :\n");
		printf("-n nom de l'hote /* par default 'localhost'*/\n");
		printf("-i adresse ip de l'hote /* par default '127.0.0.1'*/\n");
		printf("-p numero de port a utiliser /* par default '13321'*/\n");
		printf("-ps numero de port secondaire a utiliser /* par default '13322'*/\n\n");
		return 0;
	}
	if(argc < 2)
	{
		printf("Ce programme accepte les arguments. Essayez '--help' pour plus de precisions.\n\n");
	}
	printf("Lancement du serveur d'envoi.\n");
	
	hote = gethostbyname("localhost");
	if(hote == NULL)
	{
		herror("Hote inexistant ");
		return -1;
	}
	bcopy(hote->h_addr_list[0], &adServer, sizeof(adServer));
	
	server->sin_family = AF_INET;
	server->sin_addr.s_addr = adServer.s_addr;
	server->sin_port = htons(portDefault);
	*portSec = portDefault+1;
	
	while(i < argc)
	{
		if(strcmp(argv[i], "-n")==0)
		{
			hote = gethostbyname(argv[i+1]);
			if(hote == NULL)
			{
				herror("Hote inexistant ");
				return -1;
			}
			bcopy(hote->h_addr_list[0], &adServer, sizeof(adServer));
			server->sin_addr.s_addr = adServer.s_addr;
			i+=2;
		}
		else if(strcmp(argv[i], "-i")==0)
		{
			server->sin_addr.s_addr = inet_addr(argv[i+1]);
			i+=2;
		}
		else if(strcmp(argv[i], "-p")==0)
		{
			if(atoi(argv[i+1]) < 2000 || atoi(argv[i+1]) > 65535)
			{
				printf("Numero de port incorrect.\n");
				
				return -1;
			}
			server->sin_port = htons(atoi(argv[i+1]));
			i+=2;
		}
		else if(strcmp(argv[i], "-ps")==0)
		{
			if(atoi(argv[i+1]) < 2000 || atoi(argv[i+1]) > 65535)
			{
				printf("Numero de port incorrect.\n");
				
				return -1;
			}
			*portSec = atoi(argv[i+1]);
			i+=2;
		}
		else
		{
			printf("Veuillez entrer des arguments coherents.\n");
			
			return -1;
		}
	}
	
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd<0)
	{
		perror("Erreur d'ouverture de socket ");
		return -1;
	}
	
	if(bind(sd, (struct sockaddr*)server, (socklen_t)sizeof(*server)) == -1)
	{
		perror("Erreur de lien a la boite reseau ");
		close(sd);
		
		return -1;
	}
	
	printf("Adresse IP du serveur d'envoi : %s\nPort du serveur d'envoi : %d\n", (char*)inet_ntoa(server->sin_addr), htons(server->sin_port));
	
	return sd;
}

void gridCreation(char* nomExec, int* grilleShm)
{
	int mode;
	key_t key;
	
	/* Creation and initialisation of the shared memory segment */
	if((key = ftok(nomExec, 0)) == -1)
	{
		fprintf(stderr, "Executable path isn't accessible : ftok error.\n");
		exit(EXIT_FAILURE);
	}
	
	mode = IPC_CREAT|0666;
	
	if((*grilleShm = shmget(key, (W_GRILLE+1)*H_GRILLE, mode)) == -1)
	{
		perror("An error occured while creating the grid shared memory segment (shmget)");
		exit(EXIT_FAILURE);
	}
	
	if((grille = (char *)shmat(*grilleShm, NULL, 0)) == (char *)-1)
	{
		perror("An error occured while attaching the shared memory segment (shmat)");
		if(shmctl(*grilleShm, IPC_RMID, NULL) == -1)
		{
			perror("Shared memory segment's destruction impossible");
		}
		exit(EXIT_FAILURE);
	}
	
	initGrille();
	
}

/* Secondary sending threads launching function */
void* thread_broadcast(void* arg)
{
	pthread_detach(pthread_self());
	
	int i, numThread = -1, tourne = 1;
	int sd_client = *((int*)arg);
	
	printf("Client connecte sur le thread %lu avec la socket %d\n", (unsigned long)(pthread_self()), sd_client);
	
	while(tourne)
	{
		if(send(sd_client, grille, (W_GRILLE+1)*H_GRILLE, 0) < 1)
		{
			//perror("Erreur d'envoi de la grille ");
			tourne = 0;
		}
		else
		{
			sleep(FREQ_RAF);
		}
	}
	
	pthread_mutex_lock(&mutexThreads);
	printf("Client deconnecte sur le thread %lu avec la socket %d\n", (unsigned long)(pthread_self()), sd_client);
	
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
	socketClients = realloc(socketClients, nbThreads*sizeof(int));
	
	pthread_mutex_unlock(&mutexThreads);
	
	return NULL;
}


