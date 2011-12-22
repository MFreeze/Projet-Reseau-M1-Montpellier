/*
 * =====================================================================================
 *
 *       Filename:  pers_sock.h
 *
 *    Description:  Socket Library
 *
 *        Version:  1.0
 *        Created:  15/12/2011 14:36:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mfreeze
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef PERS_SOCK_H
#define PERS_SOCK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

#define ANONY_SOCK 0
#define NAMED_SOCK 1

#define DEF_PORT		13324
#define DEF_PORT_SE		13321
#define DEF_PORT_SR	13322
#define DEF_ADDR		"127.0.0.1"

typedef struct sockaddr sock_t;
typedef sock_t* p_sock_t;

typedef struct sockaddr_in sockin_t;
typedef sockin_t* p_sockin_t;

typedef struct protoent proto_t;
typedef proto_t* p_proto_t;

typedef struct hostent host_t;
typedef host_t* p_host_t;

typedef struct servent serv_t;
typedef serv_t* p_serv_t;

int create_socket_by_name (const char *hostname, const char *servname, 
		const char *protoname, const int named);

int create_socket_by_numbers (const char *ip_adr, const int num_port, 
		const int named);

int create_socket_by_hybrid (const char *hostname, const int num_port,
		const int named);

int create_socket_by_structure (const sockin_t structure, int named);

void fill_sockaddr_in (p_sockin_t addr, int domain, int port, const char *adr_ip);

#endif
