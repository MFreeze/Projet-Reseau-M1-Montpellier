
#ifndef DEF_FSERVR
#define DEF_FSERVR

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

extern int camMoving;
extern int nbMouvements;

int gstArgs(char* argv[], struct sockaddr_in *server);
void* thread_deplacement(void* arg);
char* gridRecupAddr(char* nomExec);

#endif


