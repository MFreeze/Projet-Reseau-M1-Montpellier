/*
 * =====================================================================================
 *
 *       Filename:  dm.c
 *
 *    Description:  DM oriented library implementation
 *
 *        Version:  1.0
 *        Created:  15/12/2011 16:03:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mfreeze
 *        Company:  
 *
 * =====================================================================================
 */

#include "dm.h"

void print_help() {

}

int read_options (int argc, char **argv, p_sockin_t server_adress,
		p_sockin_t pers_adress) {
	/* state can take four values :
	 *			*  0 : everything is alright
	 *			*  1 : print help
	 *			*  2 : error with the given port
	 *			*  4 : error with the given adress 
	 *			*  8 : error with the given server name 
	 *			* 16 : error with the personal port given */
	int opt, state = 0, s_port = DEF_PORT, p_port=DEF_PORT;
	p_host_t host;
	/* Initialisation de la structure du serveur */
	server_adress->sin_family = AF_INET;
	server_adress->sin_port = htons(s_port);
	inet_aton(DEF_ADR, &(server_adress->sin_addr));

	/* Initialisation de la structure du client */
	if (pers_adress) {
		pers_adress->sin_family = AF_INET;
		pers_adress->sin_port = htons(p_port);
		pers_adress->sin_addr.s_addr = htonl(INADDR_ANY);
	}

	while ((opt = getopt(argc, argv,"a:hn:p:P:")) != EOF) {
		switch (opt) {
			case 'a':
				if (inet_aton(optarg, &(server_adress->sin_addr)) == 0)
					state |= ADR_ERR | DISP_HELP;
				break;
			case 'h':
				state |= DISP_HELP;
				break;
			case 'n':
				if ((host = gethostbyname(optarg)) == NULL)
					state |= HOST_NAME_ERR | DISP_HELP;
				else
					server_adress->sin_addr.s_addr = ((struct in_addr *) host->h_addr)->s_addr;
				break;
			case 'p':
				s_port = atoi(optarg);
				if (s_port > 65535 || s_port < 0)
					state |= PORT_ERR | DISP_HELP;
				else
					server_adress->sin_port = htons(s_port);
				break;
			case 'P':
				p_port = atoi(optarg);
				if (pers_adress) {
					if (p_port > 65535 || s_port < 0)
						state |= PERS_PORT_ERR | DISP_HELP;
					else
						pers_adress->sin_port = htons(p_port);
				}
				break;
			default:
				print_help();
				break;
		}
	}
	opt = 0;

	if (state & ADR_ERR) {
		printf ("Adresse ip non valide.\n");
		opt ++;
	}
	if (state & PORT_ERR) {
		printf ("Port du serveur non valide.\n");
		opt ++;
	}
	if (state & HOST_NAME_ERR) {
		printf ("Nom d'hôte non valide.\n");
		opt ++;
	}
	if (state & PERS_PORT_ERR) {
		printf ("Port machine client non valide.\n");
		opt ++;
	}
	if (state & DISP_HELP) {
		print_help();
		opt ++;
	}

	return opt;
}
