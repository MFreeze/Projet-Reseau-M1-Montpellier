#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>

#include "gui.h"
#include "dm.h"
#include "pers_sock.h"
#include "fonctionsClient.h"


#define GRID_W 48
#define GRID_H 32
#define CLEAN()	if(close(sd) == -1){perror("Erreur de close");}

struct buff {
	int _fd_in;
	int _fd_out;
	int _pid_fils;
};

int arret = 0;
int pris_en_charge = 0;
int en_attente = 0;
int changement = 0;

/* Fonction du thread chargé de la lecture clavier */
void fn_thread (void *tube) {
	struct buff *fd = (struct buff *) tube;
	int c = 0;
	while (1) {
		c = getch();
		switch (c) {
			case 'c' :
				if (!pris_en_charge && !en_attente) {
					kill(fd->_pid_fils, SIGUSR1);
					en_attente = 1; changement = 1;
				}
				break;
			case 'q' :
				if ((!pris_en_charge && en_attente) || pris_en_charge) {
					kill(fd->_pid_fils, SIGUSR2);
					en_attente = 0; pris_en_charge = 0; changement = 1;
				}
				arret = 1;
				break;
			case KEY_LEFT:
				write (fd->_fd_out, (void *)'l', sizeof(char));
				break;
			case KEY_RIGHT:
				write (fd->_fd_out, (void *)'r', sizeof(char));
				break;
			case KEY_UP:
				write (fd->_fd_out, (void *)'u', sizeof(char));
				break;
			case KEY_DOWN:
				write (fd->_fd_out, (void *)'d', sizeof(char));
				break;
			default:
				break;
		}
		usleep(10000);
	}
}

void endloop () {
	arret = 1;
}

void connected () {
	en_attente = 0;
	pris_en_charge = 1;
	changement = 1;
}

void disconnected () {
	en_attente = 0;
	pris_en_charge = 0;
	changement = 1;
}

int main(int argc, char **argv) {
	p_host_t hote = NULL;
	sockin_t em_server, rc_server, client;
	int grid_size = GRID_H * (GRID_W + 1) + 1;
	char recvit[grid_size];
	int nbLus, sd;
	int pid = 0, tube[2];	
	int ppid = getpid();
	struct sigaction action;
	struct buff comm;

	if (pipe(tube)) {
		perror("pipe");
		return -1;
	}

	bzero(&em_server,sizeof(em_server));
	bzero(&rc_server,sizeof(rc_server));

	/* Lecture des options */
	read_options_client(argc, argv, &em_server, &rc_server);

	/* Fork du programme */
	pid = fork();

	switch(pid) {
		case -1:
			perror("fork");
			return -1;
		case 0:
#include "code_fork.c"
						break;
		default :
		/* Gestion des signaux */
		
		memset(&action, 0, sizeof(action));
		action.sa_handler = endloop;
		
		if (sigaction(SIGINT,&action,NULL)){
			perror ("sigaction");
			exit(EXIT_FAILURE);
		}

		struct sigaction action_sigusr1, action_sigusr2;
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
	}
	return EXIT_SUCCESS;
}


