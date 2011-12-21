/*
 * =====================================================================================
 *
 *       Filename:  code_pere.c
 *
 *    Description:  Code du père dans le client
 *
 *        Version:  1.0
 *        Created:  21/12/2011 08:35:32
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

if (sigaction(SIGINT,&action,NULL)){
	perror ("sigaction");
	exit(EXIT_FAILURE);
}

memset(&action_sigusr1, 0, sizeof(action_sigusr1));
memset(&action_sigusr2, 0, sizeof(action_sigusr2));
action_sigusr1.sa_handler = connected;
action_sigusr2.sa_handler = disconnected;

if (sigaction(SIGUSR2, &action_sigusr2, NULL)) {
	perror ("sigaction2");
	return -1;
}

if (sigaction(SIGUSR1, &action_sigusr1, NULL)) {
	perror ("sigaction1");
	return -1;
}

pthread_t thread;
int ret;

comm._fd_in = tube[0];
comm._fd_out = tube[1];
comm._pid_fils = pid;

if ((ret = pthread_create(&thread, NULL, (void *)&fn_thread, (void *) &comm)) != 0) {
	perror ("pthread_create");
	return -1;
}

/* Déclaration des variables de NCurse */
win_t **allwin = init_screen();

char temp[100];
sprintf (temp, "Fork effectue, pid du fils : %d", pid);
print_window(allwin[INFO_WIN], temp, 0, 0);

print_window(allwin[INFO_WIN], "Thread demarre.", 0, 0);

/* Connexion au serveur */
sd = create_socket_by_numbers (NULL, 0, ANONY_SOCK);

if(connect(sd, (struct sockaddr*)&em_server, sizeof(em_server)) == -1) {
	clean_ncurse(allwin);
	perror("Erreur de connection ");
	CLEAN()
	exit(EXIT_FAILURE);
}

print_window (allwin[INFO_WIN], "Client connecte.", 0, 0);

while(!arret)
{
	nbLus = recv(sd, recvit, grid_size, 0);
	if(nbLus < 1) {
		if(nbLus == 0)
			print_window (allwin[INFO_WIN], "Serveur deconnecte", 0, 0);
		else 
			print_window (allwin[INFO_WIN], "Erreur de reception ", 0, 0);
		arret = 1;
	}
	if (changement) {
		if (pris_en_charge) 
			print_window (allwin[INFO_WIN], "Controle de la camera acquis", 0, 0);
		else if (en_attente) 
			print_window(allwin[INFO_WIN], "En attente du controle", 0, 0);
		else
			print_window(allwin[INFO_WIN], "Deconnecte du controle de camera", 0, 0);
		changement = 0;
	}
	print_window (allwin[DISP_WIN], recvit, 2, 2);
	refresh();
}

CLEAN()

clean_ncurse(allwin);
kill (pid, SIGINT);

printf("Fermeture du client.\n");
