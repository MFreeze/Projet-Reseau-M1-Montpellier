/*
 * =====================================================================================
 *
 *       Filename:  controle_cam.c
 *
 *    Description:  Camera Controler on Server Side Implementation
 *
 *        Version:  1.0
 *        Created:  16/12/2011 10:01:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mfreeze
 *        Company:  
 *
 * =====================================================================================
 */

#include "dm.h"
#include "pers_sock.h"

int main (int argc, char **argv) {
	/* Algorithme du serveur de contrôle :
	 *	* Lecture des options
	 *	* Initialisation de la gestion des signaux
	 *	* Initialisation mémoire partagée
	 *	* Initialisation des sockets
	 *	* Acceptation d'une connexion
	 *	* Envoi d'un signal lors de la connexion
	 *	* Démarrage d'un timer
	 *	* Controle de la caméra
	 *	* Fermeture de la connexion
	 *	* Envoi du signal pour déclarer la libération du contrôle
	 *	*/

	int s_contact = 0, s_connect = 0;
	sockin_t address;
	socklen_t addr_leng = 0;

	/* Initialisation de la gestion des signaux :
	 *		* Redéfinition du comportement pour Ctrl+C et Ctrl+D
	 *		* Accepte la connexion (si possible) lors de réception de SIGUSR1
	 *		* Ferme la connexion lors de la réception de SIGUSR2 
	 *		*/

	/* Initialisation de la mémoire partagée, pas besoin de sémaphore puisqu'il y a
	 * un seul accès en écriture */

	/* Initialisation de la socket de contact */
	if ((s_contact = create_socket_by_hybrid(NULL, 1664, NAMED_SOCK)) == -1) {
		perror ("socket");
		return -1;
	}

	/* Acceptation des connexions */
	if (listen(s_contact, 5) == -1) {
		perror ("listen");
		close(s_contact);
		return -1;
	}

	addr_leng = sizeof(struct sockaddr_in);
	if ((s_connect = accept(s_contact, (struct sockaddr *)&address, &addr_leng)) < 0) {
		perror("accept");
		close(s_contact);
		return -1;
	}
	
	printf ("Connexion réussie.\n");
	
	close (s_connect);
	close (s_contact);

	return 0;
}
