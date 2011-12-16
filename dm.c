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

int read_options (int argc, char **argv, p_sockin_t server_adress) {
	/* state can take four values :
	 *			* 0 : everything is alright
	 *			* 1 : print help
	 *			* 2 : error with the given port
	 *			* 4 : error on the given adress */
	int opt, state = 0, port;
	p_host_t host;
	server_adress->sin_family = AF_INET;
	server_adress->sin_port = htons(DEF_PORT);
	inet_aton(DEF_ADR, &(server_adress->sin_addr));

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
				port = atoi(optarg);
				if (port > 65535 || port < 0)
					state |= PORT_ERR | DISP_HELP;
				else
					server_adress->sin_port = htons(port);
				break;
			case 'P':

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
		printf ("Port non valide.\n");
		opt ++;
	}
	if (state & HOST_NAME_ERR) {
		printf ("Nom d'hôte non valide.\n");
		opt ++;
	}
	if (state & DISP_HELP) {
		print_help();
		opt ++;
	}

	return opt;
}
