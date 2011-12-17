
#include "fonctionsServeurs.h"

int gstArgs(int argc, char* argv[], struct sockaddr_in *server, int portDefault)
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
		printf("-p numero de port a utiliser /* par default '13321'*/\n\n");
		return 0;
	}
	if(argc < 2)
	{
		printf("Ce programme accepte les arguments. Essayez '--help' pour plus de precisions.\n\n");
	}
	printf("Lancement du serveur.\n");
	
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
	
	printf("Adresse IP du serveur : %s\nPort du serveur : %d\n\n", (char*)inet_ntoa(server->sin_addr), htons(server->sin_port));
	
	return sd;
}

char* itoa(long n)
{
	int i = 1, nbChiffres;
	long trans = n;
	while(trans > 10)
	{
		trans /= 10;
		i++;
	}
	nbChiffres = i;
	char* chaine = malloc((nbChiffres+1) * sizeof(char));
	trans = n;
	
	for(i = 0 ; i < nbChiffres ; i++)
	{
		chaine[nbChiffres-1-i] = 48 + trans % 10;
		trans /= 10;
	}
	chaine[nbChiffres] = 0;
	
	return chaine;
}

char* gridCreation(char* nomExec, int* grilleShm, int wgrid, int hgrid)
{
	int mode, i;
	char* grille;
	key_t key;
	
	/* Creation and initialisation of the shared memory segment */
	if((key = ftok(nomExec, 0)) == -1)
	{
		fprintf(stderr, "Executable path isn't accessible : ftok error.\n");
		exit(EXIT_FAILURE);
	}
	
	mode = IPC_CREAT|0666;
	
	if((*grilleShm = shmget(key, (wgrid+1)*hgrid, mode)) == -1)
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
	
	for(i = 0 ; i < (wgrid+1)*hgrid -1 ; i++)
	{
		if((i+1) % (wgrid+1) == 0)
			grille[i] = '\n';
		else
			grille[i] = '0';
	}
	grille[(wgrid+1)*(hgrid/2)+wgrid/2] = '1';
	grille[(wgrid+1)*hgrid -1] = 0;
	
	return grille;
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

/* Secondary receving threads launching function */
void* thread_deplacement(void* arg)
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
	
	pthread_mutex_unlock(&mutexThreads);
	
	return NULL;
}

/* Signal handlers. Necessary to clean the environnement. */
void sigintHandler(int sig)
{
	printf("SIGINT received…\n");
	arret = 1;
}

void sigpipeHandler(int sig)
{
	//printf("SIGPIPE received in thread %lu\n", (unsigned long)pthread_self());
}

void attachSignals()
{
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
}

int setNonblocking(int fd)
{
    int flags;
	
    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIONBIO, &flags);
#endif
}   


