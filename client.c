

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
sockin_t em_server, rc_server, client;

void endloop () {
	arret = 1;
}


void sigpipeHandler (int sig) {
	//printf("SIG%d\n", sig);
}


int main(int argc, char **argv) {
	int grid_size = GRID_H * (GRID_W + 1) + 1;
	char recvit[grid_size], car;
	int nbLus, sd;
	struct sigaction action;
	pthread_t thread;
	int ret;
	

	/* Gestion des signaux */
	memset(&action, 0, sizeof(action));
	
	action.sa_handler = sigpipeHandler;
	if (sigaction(SIGPIPE,&action,NULL)){
		perror ("sigaction");
	}
	
	action.sa_handler = endloop;
	if (sigaction(SIGINT,&action,NULL)){
		perror ("sigaction");
		exit(EXIT_FAILURE);
	}
	
	/* Initialisation des structures reseau */
	bzero(&em_server,sizeof(em_server));
	bzero(&rc_server,sizeof(rc_server));
	bzero(&client,sizeof(client));

	/* Lecture des options */
	read_options_client(argc, argv, &em_server, &rc_server, &client);
	
	/* Connexion au serveur */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("socket");
		return -1;
	}
	if (bind(sd, (struct sockaddr *) &client, sizeof(struct sockaddr_in)) < 0) {
		perror ("bind");
		close (sd);
		return -1;
	}
	
	if(connect(sd, (struct sockaddr*)&em_server, sizeof(em_server)) == -1) {
		perror("Erreur de connection ");
		if(close(sd) == -1){perror("Erreur de close");}
		exit(EXIT_FAILURE);
	}
	
	freopen("errlog", "w+", stderr);
	
	
	printf("\nAdresse IP du serveur d'envoi : %s\nPort du serveur d'envoi : %d\n\n", (char*)inet_ntoa(em_server.sin_addr), htons(em_server.sin_port));
	printf("Adresse IP du serveur de reception : %s\nPort du serveur de reception : %d\n\n", (char*)inet_ntoa(rc_server.sin_addr), htons(rc_server.sin_port));
	printf("Adresse IP du client : %s\n\n", (char*)inet_ntoa(client.sin_addr));
	
	
	printf("Appuyez sur entree pour valider et lancer le GUI...");
	scanf("%c", &car);
	
	/* Déclaration des variables de NCurse */
	pthread_mutex_init(&mutexWin, NULL);
	allwin = init_screen();
	
	print_window (allwin[INFO_WIN], "Client connecte.", 0, 0);
	
	if ((ret = pthread_create(&thread, NULL, (void *)&fn_thread, (void *)&rc_server)) != 0) {
		perror ("pthread_create");
		return -1;
	}
	
	print_window(allwin[INFO_WIN], "Thread demarre.", 0, 0);
	
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


