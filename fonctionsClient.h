
#ifndef DEF_FCLIENT
#define DEF_FCLIENT

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
#include <fcntl.h>
#include <pthread.h>
#include <ncurses.h>
#include <signal.h>

#include "pers_sock.h"
#include "gui.h"


#define DISP_HELP				1
#define EM_SERV_PORT		2
#define SERV_ADDR				4
#define RC_SERV_PORT		8
#define HOST_NAME_ERR  16
#define CLIENT_ADDR 32
#define CLIENT_PORT 64

extern sockin_t em_server, rc_server, client;
extern win_t **allwin;
extern pthread_mutex_t mutexWin;


void print_help();
int read_options_client (int argc, char **argv, p_sockin_t em_server, p_sockin_t rc_server, p_sockin_t client);
char* itoa(long n);
void fn_thread (void *tub);

#endif


