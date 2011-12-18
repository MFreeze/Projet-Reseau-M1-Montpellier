
#ifndef DEF_FSERVE
#define DEF_FSERVE

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

#include "fonctionsServeurs.h"


extern int* socketClients;
extern int nbThreads;
extern pthread_t* thread_id;
extern pthread_mutex_t mutexThreads;
extern int grilleShm;


int gstArgs(int argc, char* argv[], struct sockaddr_in *server, int portDefault, int* portSec);
void gridCreation(char* nomExec, int* grilleShm);
void* thread_broadcast(void* arg);

#endif


