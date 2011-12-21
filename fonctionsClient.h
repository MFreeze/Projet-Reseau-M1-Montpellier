
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
#include "pers_sock.h"
#include <ncurses.h>
#include <sys/signal.h>

#define DISP_HELP				1
#define EM_SERV_PORT		2
#define SERV_ADDR				4
#define RC_SERV_PORT		8
#define HOST_NAME_ERR  16

void print_help();
int read_options_client (int argc, char **argv, p_sockin_t server_adress, 
		p_sockin_t pers_sock);
char* itoa(long n);

#endif


