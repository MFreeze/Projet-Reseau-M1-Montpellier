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

sd = create_socket_by_numbers (NULL, 0, ANONY_SOCK);
fcntl (tube[0], F_SETFL, O_NONBLOCK);

/* Démarrage de la boucle */
while (!arret) {
	int tps_connex = 0;
	int temp = 0;
	char c = 0;
	/* Attend SIGUSR1 ou 2 lui indiquant que l'utilisateur a réalisé la demande
	 * de prise en main */
	pause();
	if (!arret) {
		int cpt_con=0;
		int connecte = -1;
		/* Connexion au serveur */
		while (cpt_con++ < 3 && connecte == -1)
			connecte = connect(sd, (struct sockaddr *)&rc_server, sizeof(rc_server));
		if (connecte) {
			/* Attends la prise en charge complète du serveur */
			recv (sd, &tps_connex, sizeof(int), 0);
			/* Préviens le programme principal de la prise en charge */
			kill(ppid, SIGUSR1);

			/* Utilisation du contrôle */
			fcntl (sd, F_SETFL, O_NONBLOCK);
			while (recv(sd, &temp, sizeof(int), 0)) {
				usleep(10000);
				read(tube[0], &c, sizeof(char));
				switch (c) {
					case 'u':
					case 'd':
					case 'r':
					case 'l':
						send (sd, (void *)&c, sizeof(char), 0);
						break;
					case 'q':
						close (sd);
						kill(ppid, SIGUSR2);
						break;
					default:
						break;
				}
			}
		}
	}
}

