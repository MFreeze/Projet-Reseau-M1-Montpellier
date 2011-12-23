
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
	printf ("\tclient [-h] [-a server_address] [-n nameserveur] [-p emission_server_port] [-P recep_server_port]\n");
	printf("DESCRIPTION :\n");
	printf ("\tPermet à un utilisateur de se connecter au serveur et de demander le contrôle de la caméra.\n");
	printf("OPTIONS :\n");
	printf("\t-h, Affiche l'aide et quitte.\n");
	printf("\t-a serveur_address, Définit l'adresse IP du serveur.\n");
	printf("\t-n hostname, Retrouve l'adresse IP du serveur à partir de son nom d'hôtes.\n");
	printf("\t-p emission_server_port, Spécifie le port du serveur d'émission d'image.\n");
	printf("\t-P recep_server_port, Spécifie le port du serveur de contrôle de la caméra.\n");
}

int read_options_client (int argc, char **argv, p_sockin_t em_address,
						 p_sockin_t rc_address) {
	int opt, state = 0;
	int em_port = DEF_PORT, rc_port=DEF_PORT_R;
	p_host_t host;
	
	/* Initialisation de la structure du serveur d'émission */
	em_address->sin_family = AF_INET;
	em_address->sin_port = htons(em_port);
	inet_aton(DEF_ADDR, &em_address->sin_addr);
	
	/* Initialisation de la structure du serveur de réception */
	rc_address->sin_family = AF_INET;
	rc_address->sin_port = htons(rc_port);
	inet_aton(DEF_ADDR, &rc_address->sin_addr);
	
	while ((opt = getopt(argc, argv,"a:hn:p:P:")) != EOF) {
		switch (opt) {
			case 'a':
				if (inet_aton(optarg, &(em_address->sin_addr)) == 0)
					state |= SERV_ADDR | DISP_HELP;
				else if(inet_aton(optarg, &(rc_address->sin_addr)) == 0)
					state |= SERV_ADDR | DISP_HELP;
				break;
			case 'h':
				state |= DISP_HELP;
				break;
			case 'n':
				if ((host = gethostbyname(optarg)) == NULL)
					state |= HOST_NAME_ERR | DISP_HELP;
				else
					em_address->sin_addr.s_addr = ((struct in_addr *) host->h_addr)->s_addr;
				break;
			case 'p':
				em_port = atoi(optarg);
				if (em_port > 65535 || em_port < 0)
					state |= EM_SERV_PORT | DISP_HELP;
				else
					em_address->sin_port = htons(em_port);
				break;
			case 'P':
				rc_port = atoi(optarg);
				if (rc_address) {
					if (rc_port > 65535 || rc_port < 0)
						state |= RC_SERV_PORT | DISP_HELP;
					else
						rc_address->sin_port = htons(rc_port);
				}
				break;
			default:
				print_help();
				break;
		}
	}
	opt = 0;
	
	if (state & SERV_ADDR) {
		printf ("Adresse ip non valide.\n");
		opt ++;
	}
	if (state & EM_SERV_PORT) {
		printf ("Port du serveur non valide.\n");
		opt ++;
	}
	if (state & HOST_NAME_ERR) {
		printf ("Nom d'hôte non valide.\n");
		opt ++;
	}
	if (state & RC_SERV_PORT) {
		printf ("Port machine client non valide.\n");
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
	
	sockin_t rc_server = *((sockin_t*)tub);
	int c = 0, terminaison, cpt_con, connecte, sd;
	char buf[3] = {0};
	
	
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
			sd = create_socket_by_numbers (NULL, 0, 0);
			while (cpt_con++ < 3 && connecte == -1)
				connecte = connect(sd, (struct sockaddr *)&rc_server, sizeof(rc_server));
			if (connecte == -1) {
				fprintf(stderr, "Connexion pour le controle de la camera impossible.\n");
				pthread_mutex_lock(&mutexWin);
				print_window(allwin[INFO_WIN], "Controle de la camera refuse", 0, 0);
				refresh();
				pthread_mutex_unlock(&mutexWin);
			}
			else
			{
				/* Attends la prise en charge complète du serveur */
				if (recv (sd, buf, 3*sizeof(char), 0) > 0) {
					fprintf(stderr, "Connexion établie!\n");
					
					pthread_mutex_lock(&mutexWin);
					print_window (allwin[INFO_WIN], "Controle de la camera acquis", 0, 0);
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
						close(sd);
					
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


