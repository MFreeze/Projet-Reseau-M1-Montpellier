/*
 * =====================================================================================
 *
 *       Filename:  pers_sock.c
 *
 *    Description:  Socket Library Implementation
 *
 *        Version:  1.1
 *        Created:  15/12/2011 14:39:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mfreeze
 *        Company:  
 *
 * =====================================================================================
 */

#include "pers_sock.h"

int create_socket_by_name (const char *hostname, const char *servname, 
		const char *protoname, const int named) {
	int					sock; 
	sockin_t		structure;
	p_proto_t		protocol					= NULL;
	p_host_t		distant_machine		= NULL;
	p_serv_t		service						= NULL;

	/* Useful data retrieve */
	if (hostname) 
		if ((distant_machine = gethostbyname (hostname)) == NULL) {
			perror("gethostbyname");
			return -1;
		}

	if ((protocol = getprotobyname (protoname)) == NULL) {
		perror("getprotobyname");
		return -1;
	}

	if (servname)
		if ((service = getservbyname (servname, protocol->p_name)) == NULL) {
			perror("getservbyname");
			return -1;
		}

	/* Socket creation */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("socket");
		return -1;
	}

	if (named) {
		memset(&structure, 0, sizeof(struct sockaddr_in));
		structure.sin_family = AF_INET;
		if (servname)
			structure.sin_port = service->s_port;
		else
			structure.sin_port = htons(0);
		if (hostname)
			structure.sin_addr.s_addr = ((struct in_addr *) distant_machine->h_addr)->s_addr;
		else
			structure.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sock, (struct sockaddr *) &structure, sizeof(struct sockaddr_in)) < 0) {
			close(sock);
			perror("bind");
			return -1;
		}
	}

	return sock;	
}

int create_socket_by_numbers (const char *ip_adr, const int num_port,
		const int named) {
	int			  sock;
	sockin_t  structure;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("socket");
		return -1;
	}

	if (named) {
		fill_sockaddr_in (&structure, AF_INET, num_port, ip_adr);

		if (bind(sock, (struct sockaddr *) &structure, sizeof(struct sockaddr_in)) < 0) {
			perror ("bind");
			close (sock);
			return -1;
		}
	}

	return sock;
}

int create_socket_by_hybrid (const char *hostname, const int num_port, 
		const int	named) {
	int					sock;
	p_host_t		distant_machine = NULL;
	sockin_t		structure;

	/* Retrive Useful Informations */
	if (hostname)
		if ((distant_machine = gethostbyname (hostname)) == NULL) {
			perror("gethostbyname");
			return -1;
		}

	/* Socket Creation */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("socket");
		return -1;
	}

	/* Fill Structure if named is true */
	if (named) {
		fill_sockaddr_in (&structure, AF_INET, num_port, NULL);
		if (hostname)
			structure.sin_addr.s_addr = ((struct in_addr *) distant_machine->h_addr)->s_addr;

		if (bind(sock, (struct sockaddr *) &structure, sizeof(struct sockaddr_in)) < 0) {
			perror("bind");
			close(sock);
			return -1;
		}
	}

	return sock;
}

int create_socket_by_structure (const sockin_t address, int named){
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1) {
		perror("socket");
		return -1;
	}

	if (named) {
		if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0){
			perror ("bind");
			return -1;
		}
	}

	return sock;
}

void fill_sockaddr_in(p_sockin_t addr, int domain, int port, const char *adr_ip) {
	/* Structure Initialization */
	memset (addr, 0, sizeof (struct sockaddr_in));

	addr->sin_family = domain;
	addr->sin_port = htons(port);
	if (adr_ip)
		inet_aton (adr_ip, &(addr->sin_addr));
	else
		addr->sin_addr.s_addr = htonl(INADDR_ANY);
}
