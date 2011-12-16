
#include "fonctionsServeur.h"

void gstArgs(int argc, char* argv[], struct hostent *hote, struct sockaddr_in *server)
{
	struct in_addr adServer;
	int i = 1;
	
	if(argc == 2 && strcmp(argv[1], "--help")==0)
	{
		printf("Arguments :\n");
		printf("-n nom de l'hote /* par default 'localhost'*/\n");
		printf("-i adresse ip de l'hote /* par default '127.0.0.1'*/\n");
		printf("-p numero de port a utiliser /* par default '13321'*/\n\n");
		exit(EXIT_SUCCESS);
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
		exit(EXIT_FAILURE);
	}
	bcopy(hote->h_addr_list[0], &adServer, sizeof(adServer));
	
	server->sin_family = AF_INET;
	server->sin_addr.s_addr = adServer.s_addr;
	server->sin_port = htons(13321);
	
	while(i < argc)
	{
		if(strcmp(argv[i], "-n")==0)
		{
			hote = gethostbyname(argv[i+1]);
			if(hote == NULL)
			{
				herror("Hote inexistant ");
				exit(EXIT_FAILURE);
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
				exit(EXIT_FAILURE);
			}
			server->sin_port = htons(atoi(argv[i+1]));
			i+=2;
		}
		else
		{
			printf("Veuillez entrer des arguments coherents.\n");
			exit(EXIT_FAILURE);
		}
	}
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

int backupClients(int* clients, int** clientsTrans, int nbClients)
{
	int j;
	*clientsTrans = malloc(sizeof(int)*nbClients);
	if(*clientsTrans == NULL)
	{
		perror("Erreur d'allocation ");
		return -1;
	}
	for(j = 0 ; j < nbClients ; j++)
	{
		(*clientsTrans)[j] = clients[j];
	}
	return 0;
}

int recupClients(int** clients, int** clientsTrans, int nbClients, int sd_client)
{
	int j;
	*clients = realloc(*clients, sizeof(int)*nbClients);
	if(*clients == NULL)
	{
		perror("Erreur de reallocation ");
		return -1;
	}
	(*clients)[nbClients-1] = sd_client;
	for(j = 0 ; j < nbClients-1 ; j++)
	{
		(*clients)[j] = (*clientsTrans)[j];
	}
	free(*clientsTrans);
	return 0;
}





