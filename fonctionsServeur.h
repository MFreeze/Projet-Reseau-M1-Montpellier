
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

void gstArgs(int argc, char* argv[], struct hostent *hote, struct sockaddr_in *server);
char* itoa(long n);
int backupClients(int* clients, int** clientsTrans, int nbClients);
int recupClients(int** clients, int** clientsTrans, int nbClients, int sd_client);
void* gestionClient(void * arg);


