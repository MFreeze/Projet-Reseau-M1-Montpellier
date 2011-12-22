
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
#define FREQ_RAF 1

extern char* grille;
extern int arret;


char* itoa(long n);
void attachSignals();
int setNonblocking(int fd);
void initGrille();

#endif DEF_FSERV


