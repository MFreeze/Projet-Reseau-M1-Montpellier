/*
 * =====================================================================================
 *
 *       Filename:  dm.h
 *
 *    Description:  DM oriented library
 *
 *        Version:  1.0
 *        Created:  15/12/2011 16:01:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mfreeze
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef DM_H
#define DM_H

#define DISP_HELP				1
#define EM_SERV_PORT		2
#define SERV_ADDR				4
#define RC_SERV_PORT		8
#define HOST_NAME_ERR  16

#include "pers_sock.h"
#include <ncurses.h>
#include <sys/signal.h>

void print_help();
int read_options_client (int argc, char **argv, p_sockin_t server_adress, 
		p_sockin_t pers_sock);

#endif 
