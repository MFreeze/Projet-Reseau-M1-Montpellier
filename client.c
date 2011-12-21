

#include "fonctionsClient.h"


#define GRID_W 48
#define GRID_H 32

struct buff {
	int _fd_in;
	int _fd_out;
	int _pid_fils;
};

int arret = 0;
win_t **allwin;
pthread_mutex_t mutexWin;

void endloop () {
	arret = 1;
}


void sigpipeHandler (int sig) {
	//printf("SIG%d\n", sig);
}


int main(int argc, char **argv) {
	sockin_t em_server, rc_server, client;
	int grid_size = GRID_H * (GRID_W + 1) + 1;
	char recvit[grid_size];
	int nbLus, sd, i;
	int sommeil = 1;
	struct sigaction action;
	pthread_t thread;
	int ret;
	/*char nomLog[10] = {0};
	char* trans;*/
	

	bzero(&em_server,sizeof(em_server));
	bzero(&rc_server,sizeof(rc_server));

	/* Lecture des options */
	read_options_client(argc, argv, &em_server, &rc_server);
	
	/*printf("\nAdresse IP du serveur d'envoi : %s\nPort du serveur d'envoi : %d\n\n", (char*)inet_ntoa(em_server.sin_addr), htons(em_server.sin_port));
	printf("Adresse IP du serveur de reception : %s\nPort du serveur de reception : %d\n\n", (char*)inet_ntoa(rc_server.sin_addr), htons(rc_server.sin_port));*/

	/* Gestion des signaux */
	memset(&action, 0, sizeof(action));
	
	action.sa_handler = sigpipeHandler;
	if (sigaction(SIGPIPE,&action,NULL)){
		perror ("sigaction");
	}
	/*for(i = 0 ; i < 33 ; i++)
	{
	if (sigaction(i,&action,NULL)){
		perror ("sigaction");
	}
	}*/
	
	action.sa_handler = endloop;
	if (sigaction(SIGINT,&action,NULL)){
		perror ("sigaction");
		exit(EXIT_FAILURE);
	}
	
	/* Connexion au serveur */
	sd = create_socket_by_numbers (NULL, 0, ANONY_SOCK);
	
	if(connect(sd, (struct sockaddr*)&em_server, sizeof(em_server)) == -1) {
		clean_ncurse(allwin);
		perror("Erreur de connection ");
		if(close(sd) == -1){perror("Erreur de close");}
		exit(EXIT_FAILURE);
	}
	
	/*strcpy(nomLog, "errlog");
	trans = itoa(sd);
	strcat(nomLog, trans);
	free(trans);*/
	freopen("errlog", "w+", stderr);
	
	/* Déclaration des variables de NCurse */
	pthread_mutex_init(&mutexWin, NULL);
	allwin = init_screen();
	
	char temp[100];
	print_window (allwin[INFO_WIN], "Client connecte.", 0, 0);
	
	if ((ret = pthread_create(&thread, NULL, (void *)&fn_thread, (void *)&rc_server)) != 0) {
		perror ("pthread_create");
		return -1;
	}
	
	print_window(allwin[INFO_WIN], "Thread demarre.", 0, 0);
	int tempi = 0;
	
	while(!arret)
	{
		nbLus = recv(sd, recvit, grid_size, 0);
		if(nbLus < 1) {
			if(nbLus == 0 || (nbLus == -1 && errno != EINTR))
				arret = 1;
		}
		pthread_mutex_lock(&mutexWin);
		print_window (allwin[DISP_WIN], recvit, 2, 2);
		refresh();
		pthread_mutex_unlock(&mutexWin);
	}
	
	if(close(sd) == -1){perror("Erreur de close");}
	
	pthread_kill(thread, SIGINT);
	clean_ncurse(allwin);
	
	printf("Fermeture du client.\n");
	return EXIT_SUCCESS;
}


