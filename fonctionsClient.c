
#include "fonctionsClient.h"

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

void print_help() {
	printf ("SYNOPSIS :\n");
	printf ("\tclient [-h] [-a client_address] [-A server_address] [-P emission_server_port] [-S recep_server_port]\n");
	printf("DESCRIPTION :\n");
	printf ("\tPermet à un utilisateur de se connecter au serveur et de demander le contrôle de la caméra.\n");
	printf("OPTIONS :\n");
	printf("\t-h, Affiche l'aide et quitte.\n");
	printf("\t-a client_address, Définit l'adresse IP du client.\n");
	printf("\t-A serveur_address, Définit l'adresse IP du serveur.\n");
	printf("\t-n hostname, Retrouve l'adresse IP du client à partir de son nom d'hôte.\n");
	printf("\t-N hostname, Retrouve l'adresse IP du serveur à partir de son nom d'hôte.\n");
	printf("\t-P emission_server_port, Spécifie le port du serveur d'émission d'image.\n");
	printf("\t-S recep_server_port, Spécifie le port du serveur de contrôle de la caméra.\n");
}

int read_options_client (int argc, char **argv, p_sockin_t em_server,
						 p_sockin_t rc_server, p_sockin_t client) {
	int opt, state = 0;
	int em_port = DEF_PORT_SE, rc_port=DEF_PORT_SR;
	p_host_t host;
	
	/* Initialisation de la structure du serveur d'émission */
	em_server->sin_family = AF_INET;
	em_server->sin_port = htons(em_port);
	inet_aton(DEF_ADDR, &em_server->sin_addr);
	
	/* Initialisation de la structure du serveur de réception */
	rc_server->sin_family = AF_INET;
	rc_server->sin_port = htons(rc_port);
	inet_aton(DEF_ADDR, &rc_server->sin_addr);
	
	/* Initialisation de la structure du client */
	client->sin_family = AF_INET;
	inet_aton(DEF_ADDR, &client->sin_addr);
	
	while ((opt = getopt(argc, argv,"a:A:hn:N:P:S:")) != EOF) {
		switch (opt) {
			case 'a':
				if (inet_aton(optarg, &(client->sin_addr)) == 0)
					state |= CLIENT_ADDR | DISP_HELP;
				break;
			case 'A':
				if (inet_aton(optarg, &(em_server->sin_addr)) == 0)
					state |= SERV_ADDR | DISP_HELP;
				else if(inet_aton(optarg, &(rc_server->sin_addr)) == 0)
					state |= SERV_ADDR | DISP_HELP;
				break;
			case 'h':
				state |= DISP_HELP;
				break;
			case 'n':
				if ((host = gethostbyname(optarg)) == NULL)
					state |= HOST_NAME_ERR | DISP_HELP;
				else
					client->sin_addr.s_addr = ((struct in_addr *) host->h_addr)->s_addr;
				break;
			case 'N':
				if ((host = gethostbyname(optarg)) == NULL)
					state |= HOST_NAME_ERR | DISP_HELP;
				else
				{
					em_server->sin_addr.s_addr = ((struct in_addr *) host->h_addr)->s_addr;
					rc_server->sin_addr.s_addr = ((struct in_addr *) host->h_addr)->s_addr;
				}
				break;
			case 'P':
				em_port = atoi(optarg);
				if (em_port > 65535 || em_port < 0)
					state |= EM_SERV_PORT | DISP_HELP;
				else
					em_server->sin_port = htons(em_port);
				break;
			case 'S':
				rc_port = atoi(optarg);
				if (rc_server) {
					if (rc_port > 65535 || rc_port < 0)
						state |= RC_SERV_PORT | DISP_HELP;
					else
						rc_server->sin_port = htons(rc_port);
				}
				break;
			default:
				print_help();
				break;
		}
	}
	opt = 0;
	
	if (state & SERV_ADDR) {
		printf ("Adresse ip du serveur non valide.\n");
		opt ++;
	}
	if (state & CLIENT_ADDR) {
		printf ("Adresse ip du client non valide.\n");
		opt ++;
	}
	if (state & EM_SERV_PORT) {
		printf ("Port du serveur d'envoi non valide.\n");
		opt ++;
	}
	if (state & RC_SERV_PORT) {
		printf ("Port du serveur de reception non valide.\n");
		opt ++;
	}
	if (state & CLIENT_PORT) {
		printf ("Port machine client non valide.\n");
		opt ++;
	}
	if (state & HOST_NAME_ERR) {
		printf ("Nom d'hôte non valide.\n");
		opt ++;
	}
	if (state & DISP_HELP) {
		print_help();
		opt ++;
	}
	
	
	return opt;
}


/* Fonction du thread chargé de la lecture clavier */
void fn_thread (void *tub) {
	pthread_detach(pthread_self());
	
	int c = 0, terminaison, cpt_con, connecte, sd;
	char buf[3] = {0};
	char erreur[128];
	char temp[100];
	
	while (1) {
		c = getch();
		
		if(c == 'c')
		{
			pthread_mutex_lock(&mutexWin);
			print_window(allwin[INFO_WIN], "En attente du controle", 0, 0);
			refresh();
			pthread_mutex_unlock(&mutexWin);
			cpt_con=0;
			connecte = -1;
			
			/* Connexion au serveur */
			if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
				perror ("socket");
			}
			if (bind(sd, (struct sockaddr *) &client, sizeof(struct sockaddr_in)) < 0) {
				perror ("bind");
				close (sd);
			}
			
			while (cpt_con++ < 3 && connecte == -1)
				connecte = connect(sd, (struct sockaddr *)&rc_server, sizeof(rc_server));
			if (connecte == -1) {
				fprintf(stderr, "Connexion pour le controle de la camera impossible.\n");
				pthread_mutex_lock(&mutexWin);
				strcpy(erreur, "Controle de la camera refuse : ");
				print_window(allwin[INFO_WIN], erreur, 0, 0);
				strcpy(erreur, strerror(errno));
				print_window(allwin[INFO_WIN], erreur, 0, 0);
				refresh();
				pthread_mutex_unlock(&mutexWin);
				close(sd);
			}
			else
			{
				/* Attends la prise en charge complète du serveur */
				if (recv (sd, buf, 3*sizeof(char), 0) > 0) {
					fprintf(stderr, "Connexion établie!\n");
					
					sprintf (temp, "Controle de la camera acquis pour moins de %d sec", atoi(buf));
					pthread_mutex_lock(&mutexWin);
					print_window (allwin[INFO_WIN], temp, 0, 0);
					refresh();
					pthread_mutex_unlock(&mutexWin);
					
					terminaison = 0;
					buf[1] = 0; buf[2] = 0;
					c = getch();
					while (c != 'q' && !terminaison) {
						
						switch (c) {
							case KEY_LEFT:
								fprintf(stderr, "Recu Left\n");
								buf[0] = 'l';
								if(send (sd, buf, 3*sizeof(char), 0) == -1)
								{
									terminaison = 1;
								}
								break;
							case KEY_RIGHT:
								fprintf(stderr, "Recu Right\n");
								buf[0] = 'r';
								if(send (sd, buf, 3*sizeof(char), 0) == -1)
								{
									terminaison = 1;
								}
								break;
							case KEY_UP:
								fprintf(stderr, "Recu Up\n");
								buf[0] = 'u';
								if(send (sd, buf, 3*sizeof(char), 0) == -1)
								{
									terminaison = 1;
								}
								break;
							case KEY_DOWN:
								fprintf(stderr, "Recu Down\n");
								buf[0] = 'd';
								if(send (sd, buf, 3*sizeof(char), 0) == -1)
								{
									terminaison = 1;
								}
								break;
							default:
								break;
						}
						if(!terminaison)
							c = getch();
					}
					if(c == 'q')
					{
						buf[0] = 'q';
						send (sd, buf, 3*sizeof(char), 0);
						close(sd);
					}
					
					pthread_mutex_lock(&mutexWin);
					print_window(allwin[INFO_WIN], "Deconnecte du controle de camera", 0, 0);
					refresh();
					pthread_mutex_unlock(&mutexWin);
				}
			}
		}
	}
	printf("Terminaison du thread\n");
	pthread_exit(NULL);
}

