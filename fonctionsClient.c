
#include "fonctionsClient.h"

void gstArgs(int argc, char* argv[], struct hostent *hote, struct sockaddr_in *server, struct sockaddr_in *client)
{
	struct in_addr adresse;
	int i = 1;
	
	if(argc == 2 && strcmp(argv[1], "--help")==0)
	{
		printf("Arguments :\n");
		printf("-N nom du serveur /* par default 'localhost'*/\n");
		printf("-I adresse ip du serveur /* par default '127.0.0.1'*/\n");
		printf("-P numero de port du serveur /* par default '13321'*/\n");
		printf("-p numero de port du client /* par default '13324'*/\n\n");
		
		exit(EXIT_SUCCESS);
	}
	else if(argc < 2)
	{
		printf("Ce programme accepte les arguments. Essayez '--help' pour plus de precisions.\n\n");
	}
	else if(argc % 2 == 0)
	{
		printf("Veuillez entrer des arguments coherents.\n");
		
		exit(EXIT_FAILURE);
	}
	
	printf("Lancement du client.\n");
	
	hote = gethostbyname("localhost");
	if(hote == NULL)
	{
		printf("Hote inexistant\n");
		
		exit(EXIT_FAILURE);
	}
	bcopy(hote->h_addr_list[0], &adresse, sizeof(adresse));
	
	server->sin_family = AF_INET;
	server->sin_addr.s_addr = adresse.s_addr;
	server->sin_port = htons(13321);
	
	client->sin_family = AF_INET;
	client->sin_addr.s_addr = adresse.s_addr;
	client->sin_port = htons(13324);
	
	while(i < argc)
	{
		if(i == argc-1)
		{
			printf("Veuillez entrer des arguments coherents.\n");
			
			exit(EXIT_FAILURE);
		}
		if(strcmp(argv[i], "-N")==0)
		{
			hote = gethostbyname(argv[i+1]);
			if(hote == NULL)
			{
				printf("Hote inexistant\n");
				
				exit(EXIT_FAILURE);
			}
			bcopy(hote->h_addr_list[0], &adresse, sizeof(adresse));
			server->sin_addr.s_addr = adresse.s_addr;
			i+=2;
		}
		else if(strcmp(argv[i], "-I")==0)
		{
			server->sin_addr.s_addr = inet_addr(argv[i+1]);
			i+=2;
		}
		else if(strcmp(argv[i], "-P")==0)
		{
			if(atoi(argv[i+1]) < 2000 || atoi(argv[i+1]) > 65535)
			{
				printf("Numero de port incorrect.\n");
				
				exit(EXIT_FAILURE);
			}
			server->sin_port = htons(atoi(argv[i+1]));
			i+=2;
		}
		else if(strcmp(argv[i], "-p")==0)
		{
			if(atoi(argv[i+1]) < 2000 || atoi(argv[i+1]) > 65535)
			{
				printf("Numero de port incorrect.\n");
				
				exit(EXIT_FAILURE);
			}
			client->sin_port = htons(atoi(argv[i+1]));
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


