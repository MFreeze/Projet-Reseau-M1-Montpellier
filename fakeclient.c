/*
 * =====================================================================================
 *
 *       Filename:  fakeclient.c
 *
 *    Description:  Temporary File
 *
 *        Version:  1.0
 *        Created:  16/12/2011 11:16:27
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
	int s_contact = 0;
	sockin_t to_connect;
	socklen_t addr_len = 0;

	if ((s_contact = create_socket_by_hybrid(NULL, 9861, ANONY_SOCK)) == -1) {
		perror ("socket");
		return -1;
	}

	addr_len = sizeof (struct sockaddr_in);
	memset (&to_connect, 0, addr_len);

	fill_sockaddr_in(&to_connect, AF_INET, 1664, "127.0.0.1");
	if (connect (s_contact, (struct sockaddr *)&to_connect, addr_len) == -1) {
		perror ("connect");
		close (s_contact);
		return -1;
	}

	printf ("Connected\n");

	close (s_contact);
	return 0;
}
