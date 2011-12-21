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
int tube[2] = {0, 0};

/* Fonction du thread chargé de la lecture clavier */
void fn_thread (void *tub) {
	int pid = (int)tub;
	int c = 0;
	char buf[2] = {0};
	fprintf (stderr, "Tube thread : %d %d\n", tube[1], tube[0]);
	while (1) {
		c = getch();
		switch (c) {
			case 'c' :
				if (!pris_en_charge && !en_attente) {
					kill(pid, SIGUSR1);
					en_attente = 1; changement = 1;
				}
				break;
			case 'q' :
				if ((!pris_en_charge && en_attente) || pris_en_charge) {
					buf[0] = 'q';
					if (write (tube[1], buf, 2*sizeof(char)) < 1)
						fprintf (stderr, "Plantage\n");
					en_attente = 0; pris_en_charge = 0; changement = 1;
				}
				break;
			case KEY_LEFT:
				fprintf(stderr, "Recu Left\n");
				buf[0] = 'l';
				if (write (tube[1], buf, 2*sizeof(char)) < 1)
					fprintf (stderr, "Plantage !\n");
				break;
			case KEY_RIGHT:
				fprintf(stderr, "Recu Right\n");
				buf[0] = 'r';
				if (write (tube[1], buf, 2*sizeof(char)) < 1) 
					fprintf(stderr, "Plantage \n");
				break;
			case KEY_UP:
				fprintf(stderr, "Recu Up\n");
				buf[0] = 'u';
				if (write (tube[1], buf, 2*sizeof(char)) < 1) 
					fprintf(stderr, "Plantage \n");
				break;
			case KEY_DOWN:
				fprintf(stderr, "Recu Down\n");
				buf[0] = 'd';
				if (write (tube[1], buf, 2*sizeof(char)) < 1) 
					fprintf(stderr, "Plantage \n");
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

void endwait() {
	fprintf (stderr, "Signal SIGUSR1 reçu...\n");
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
	int sommeil = 1;
	int pid = 0;	
	int ppid = getpid();
	struct sigaction action;
	struct sigaction action_sigusr1, action_sigusr2;

	if (pipe(tube)) {
		perror("pipe");
		return -1;
	}
	
	freopen("errlog", "w+", stderr);

	bzero(&em_server,sizeof(em_server));
	bzero(&rc_server,sizeof(rc_server));

	/* Lecture des options */
	read_options_client(argc, argv, &em_server, &rc_server);
	
	/*printf("\nAdresse IP du serveur d'envoi : %s\nPort du serveur d'envoi : %d\n\n", (char*)inet_ntoa(em_server.sin_addr), htons(em_server.sin_port));
	printf("Adresse IP du serveur de reception : %s\nPort du serveur de reception : %d\n\n", (char*)inet_ntoa(rc_server.sin_addr), htons(rc_server.sin_port));*/

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
			#include "code_pere.c"
			break;
	}
	return EXIT_SUCCESS;
}


