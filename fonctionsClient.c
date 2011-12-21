
#include "fonctionsClient.h"

char* itoa(long n)
{
	int i = 1, nbChiffres;
	long trans = n;
	while(trans > 10)
	{
		trans /= 10;
		i++;
	}
	nbChiffres = i;
	char* chaine = malloc((nbChiffres+1) * sizeof(char));
	trans = n;
	
	for(i = 0 ; i < nbChiffres ; i++)
	{
		chaine[nbChiffres-1-i] = 48 + trans % 10;
		trans /= 10;
	}
	chaine[nbChiffres] = 0;
	
	return chaine;
}

void print_help() {

}

int read_options_client (int argc, char **argv, p_sockin_t em_address,
		p_sockin_t rc_address) {
	int opt, state = 0;
	int em_port = DEF_PORT, rc_port=DEF_PORT_R;
	p_host_t host;

	/* Initialisation de la structure du serveur d'émission */
	em_address->sin_family = AF_INET;
	em_address->sin_port = htons(em_port);
	inet_aton(DEF_ADDR, &em_address->sin_addr);

	/* Initialisation de la strcutre du serveur de réception */
	rc_address->sin_family = AF_INET;
	rc_address->sin_port = htons(rc_port);
	inet_aton(DEF_ADDR, &rc_address->sin_addr);

	while ((opt = getopt(argc, argv,"a:hn:p:P:")) != EOF) {
		switch (opt) {
			case 'a':
				if (inet_aton(optarg, &(em_address->sin_addr)) == 0)
					state |= SERV_ADDR | DISP_HELP;
				else if(inet_aton(optarg, &(rc_address->sin_addr)) == 0)
					state |= SERV_ADDR | DISP_HELP;
				break;
			case 'h':
				state |= DISP_HELP;
				break;
			case 'n':
				if ((host = gethostbyname(optarg)) == NULL)
					state |= HOST_NAME_ERR | DISP_HELP;
				else
					em_address->sin_addr.s_addr = ((struct in_addr *) host->h_addr)->s_addr;
				break;
			case 'p':
				em_port = atoi(optarg);
				if (em_port > 65535 || em_port < 0)
					state |= EM_SERV_PORT | DISP_HELP;
				else
					em_address->sin_port = htons(em_port);
				break;
			case 'P':
				rc_port = atoi(optarg);
				if (rc_address) {
					if (rc_port > 65535 || rc_port < 0)
						state |= RC_SERV_PORT | DISP_HELP;
					else
						rc_address->sin_port = htons(rc_port);
				}
				break;
			default:
				print_help();
				break;
		}
	}
	opt = 0;

	if (state & SERV_ADDR) {
		printf ("Adresse ip non valide.\n");
		opt ++;
	}
	if (state & EM_SERV_PORT) {
		printf ("Port du serveur non valide.\n");
		opt ++;
	}
	if (state & HOST_NAME_ERR) {
		printf ("Nom d'hôte non valide.\n");
		opt ++;
	}
	if (state & RC_SERV_PORT) {
		printf ("Port machine client non valide.\n");
		opt ++;
	}
	if (state & DISP_HELP) {
		print_help();
		opt ++;
	}

	return opt;
}
