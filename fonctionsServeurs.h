
#ifndef DEF_FSERV
#define DEF_FSERV

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <xlocale.h>
#include <pthread.h>
#include <signal.h>

#define W_GRILLE 7
#define H_GRILLE 5
#define FREQ_RAF 2


extern int* socketClients;
extern int nbThreads;
extern pthread_t* thread_id;
extern pthread_mutex_t mutexThreads;
extern int grilleShm;
extern char* grille;
extern int arret;


int gstArgs(int argc, char* argv[], struct sockaddr_in *server, int portDefault);
char* itoa(long n);
char* gridCreation(char* nomExec, int* grilleShm, int wgrid, int hgrid);
void* thread_broadcast(void* arg);
void* thread_deplacement(void* arg);
void attachSignals();
int setNonblocking(int fd);

#endif DEF_FSERV


