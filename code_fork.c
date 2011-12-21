/*
 * =====================================================================================
 *
 *       Filename:  code_fork.c
 *
 *    Description:  Code de la Partie forkée du client
 *
 *        Version:  1.0
 *        Created:  20/12/2011 14:09:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mfreeze
 *        Company:  
 *
 * =====================================================================================
 */

/* Gestion des signaux */
memset(&action, 0, sizeof(action));
action.sa_handler = endloop;
if (sigaction(SIGINT, &action, NULL)) {
	perror ("sigaction proc 2");
	return -1;
}

memset(&action_sigusr1, 0, sizeof(action));
action.sa_handler = endwait;
if (sigaction(SIGUSR1, &action, NULL)) {
	perror ("sigusr1 fils");
	return -1;
}

fcntl (tube[0], F_SETFL, O_NONBLOCK);

/* Démarrage de la boucle */
while (!arret) {
	int tps_connex = 0;
	int temp = 0, terminaison_boucle = 0;
	char c = 0;
	char buffer[3];
	sd = create_socket_by_numbers (NULL, 0, ANONY_SOCK);
	/* Attend SIGUSR1 ou 2 lui indiquant que l'utilisateur a réalisé la demande
	 * de prise en main */
	fprintf (stderr, "Test\n");
	pause();
	fprintf (stderr, "Signal reçu\n");
	if (!arret) {
		int cpt_con=0;
		int connecte = -1;
		/* Connexion au serveur */
		while (cpt_con++ < 3 && connecte == -1)
			connecte = connect(sd, (struct sockaddr *)&rc_server, sizeof(rc_server));
		if (!connecte) {
			fprintf(stderr, "Connexion établie!\n");
			/* Attends la prise en charge complète du serveur */
			if (recv (sd, buffer, 3*sizeof(char), 0) > 0) {
				tps_connex = atoi(buffer);
				sleep(2);
				/* Préviens le programme principal de la prise en charge */
				kill(ppid, SIGUSR1);

				fprintf(stderr, "Pris en charge...\n");

				/* Utilisation du contrôle */
				fcntl (sd, F_SETFL, O_NONBLOCK);
				while (recv(sd, &temp, sizeof(int), 0) && !terminaison_boucle) {
					usleep(10000);
					c = ' ';
					if (read(tube[0], &c, sizeof(char))!=-1) {
					switch (c) {
						case 'u':
							fprintf (stderr, "Lu u\n");
							send (sd, (void *)&c, sizeof(char), 0);
							break;
						case 'd':
							fprintf (stderr, "Lu d\n");
							send (sd, (void *)&c, sizeof(char), 0);
							break;
						case 'r':
							fprintf (stderr, "Lu r\n");
							send (sd, (void *)&c, sizeof(char), 0);
							break;
						case 'l':
							fprintf(stderr, "Envoi en cours.\n");
							send (sd, (void *)&c, sizeof(char), 0);
							break;
						case 'q':
							fprintf(stderr, "Réception q\n");
							terminaison_boucle ++;
							break;
						default:
							break;
					}
				}
				}
			}
		}
	}
	kill(ppid, SIGUSR2);
	close(sd);
}

