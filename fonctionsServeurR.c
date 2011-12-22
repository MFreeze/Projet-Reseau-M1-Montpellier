
#include "fonctionsServeurR.h"

int gstArgs(char* argv[], struct sockaddr_in *server)
{
	int sd = -1;
	
	bzero(server,sizeof(*server));
	printf("Lancement du serveur de reception.\n");
	
	
	server->sin_family = AF_INET;
	server->sin_addr.s_addr = atoi(argv[2]);
	server->sin_port = htons(atoi(argv[3]));
		
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
	
	printf("Adresse IP du serveur de reception : %s\nPort du serveur de reception : %d\n", (char*)inet_ntoa(server->sin_addr), htons(server->sin_port));
	
	return sd;
}

/* Secondary receving threads launching function */
void* thread_deplacement(void* arg)
{
	int tourne = 1, nbLus, i, xPoint, yPoint;
	pthread_mutex_lock(&mutexSockets);
	int sd_client = socketClients[0];
	pthread_mutex_unlock(&mutexSockets);
	char buffer[3];
	

	printf("Le client sur la socket %d prend la controle de la camera.\n", sd_client);
	
	/* calcul de la position du pointeur */
	for(i = 0 ; i < (W_GRILLE+1)*H_GRILLE ; i++)
	{
		if(grille[i] == '1')
		{
			xPoint = i % (W_GRILLE+1);
			yPoint = i / (W_GRILLE+1);
		}
	}
	
	sprintf(buffer, "%d", 1);
	
	/* signification de la connection au client */
	if(send(sd_client, buffer, 3*sizeof(char), 0) == -1)
	{
		perror("Signification de la connection au client en erreur ");
	}
	else
		printf("Signification de la connection au client effectuee.\n");
	
	
	while(tourne)
	{
		if(nbMouvements >= 50)
		{
			initGrille();
			xPoint = ((W_GRILLE+1)*(H_GRILLE/2)+W_GRILLE/2) % (W_GRILLE+1);
			yPoint = ((W_GRILLE+1)*(H_GRILLE/2)+W_GRILLE/2) / (W_GRILLE+1);
			nbMouvements = 0;
			printf("Grille reinitialisee apres 50 mouvements.\n");
		}
		nbLus = recv(sd_client, buffer, 3*sizeof(char), 0);
		if(nbLus < 1)
		{
			if(errno == 35)
				continue;
			//perror("Erreur de lecture ");
			//printf("errno : %d\n", errno);
			else
				tourne = 0;
		}
		else
		{
			if(strchr(buffer, 'r') != NULL)
			{
				if(xPoint < W_GRILLE-1)
				{
					grille[(W_GRILLE+1)*yPoint + xPoint] = '0';
					xPoint++;
					grille[(W_GRILLE+1)*yPoint + xPoint] = '1';
				}
				printf("Deplacement du pointeur vers la droite.\n");
			}
			if(strchr(buffer, 'l') != NULL)
			{
				if(xPoint > 0)
				{
					grille[(W_GRILLE+1)*yPoint + xPoint] = '0';
					xPoint--;
					grille[(W_GRILLE+1)*yPoint + xPoint] = '1';
				}
				printf("Deplacement du pointeur vers la gauche.\n");
			}
			if(strchr(buffer, 'u') != NULL)
			{
				if(yPoint > 0)
				{
					grille[(W_GRILLE+1)*yPoint + xPoint] = '0';
					yPoint--;
					grille[(W_GRILLE+1)*yPoint + xPoint] = '1';
				}
				printf("Deplacement du pointeur vers le haut.\n");
			}
			if(strchr(buffer, 'd') != NULL)
			{
				if(yPoint < H_GRILLE-1)
				{
					grille[(W_GRILLE+1)*yPoint + xPoint] = '0';
					yPoint++;
					grille[(W_GRILLE+1)*yPoint + xPoint] = '1';
				}
				printf("Deplacement du pointeur vers la bas.\n");
			}
			nbMouvements++;
		}
	}
	
	printf("Le client sur la socket %d perd la controle de la camera.\n", sd_client);
	pthread_mutex_lock(&mutexSockets);
	for(i = 0 ; i < nbClients-1 ; i++)
	{
		socketClients[i] = socketClients[i+1];
	}
	nbClients--;
	if(nbClients == 0)
	{
		socketClients = NULL;
		printf("Plus de clients sur la liste d'attente.\n");
	}
	else
	{
		socketClients = realloc(socketClients, nbClients*sizeof(int));
		printf("Clients restants sur la liste d'attente : ");
		for(i = 0 ; i < nbClients ; i++)
		{
			printf("%d ", socketClients[i]);
		}
		printf("\n");
	}
	
	
	pthread_mutex_unlock(&mutexSockets);
	camMoving = 0;
	
	pthread_exit(NULL);
}

char* gridRecupAddr(char* nomExec)
{
	int grilleShm;
	int mode;
	char* grille;
	key_t key;
	
	/* Creation and initialisation of the shared memory segment */
	if((key = ftok(nomExec, 0)) == -1)
	{
		fprintf(stderr, "Executable path isn't accessible : ftok error.\n");
		exit(EXIT_FAILURE);
	}
	
	mode = 0666;
	
	if((grilleShm = shmget(key, (W_GRILLE+1)*H_GRILLE, mode)) == -1)
	{
		perror("An error occured while getting the grid shared memory segment (shmget)");
		exit(EXIT_FAILURE);
	}
	
	if((grille = (char *)shmat(grilleShm, NULL, 0)) == (char *)-1)
	{
		perror("An error occured while attaching the shared memory segment (shmat)");
		if(shmctl(grilleShm, IPC_RMID, NULL) == -1)
		{
			perror("Shared memory segment's destruction impossible");
		}
		exit(EXIT_FAILURE);
	}
	
	return grille;
}


