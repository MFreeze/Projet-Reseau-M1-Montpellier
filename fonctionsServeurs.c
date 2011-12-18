
#include "fonctionsServeurs.h"

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

/* Signal handlers. Necessary to clean the environnement. */
void sigintHandler(int sig)
{
	printf("SIGINT received…\n");
	arret = 1;
}

void sigpipeHandler(int sig)
{
	//printf("SIGPIPE received in thread %lu\n", (unsigned long)pthread_self());
}

void attachSignals()
{
	struct sigaction action;
	
	memset(&action, 0, sizeof(action));
	action.sa_handler = sigintHandler;
	if (sigaction(SIGINT,&action,NULL))
	{
		perror("sigaction");
	}
	action.sa_handler = sigpipeHandler;
	if (sigaction(SIGPIPE,&action,NULL))
	{
		perror("sigaction");
	}
}

int setNonblocking(int fd)
{
    int flags;
	
    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIONBIO, &flags);
#endif
}

void initGrille()
{
	int i;
	for(i = 0 ; i < (W_GRILLE+1)*H_GRILLE -1 ; i++)
	{
		if((i+1) % (W_GRILLE+1) == 0)
			grille[i] = '\n';
		else
			grille[i] = '0';
	}
	grille[(W_GRILLE+1)*(H_GRILLE/2)+W_GRILLE/2] = '1';
	grille[(W_GRILLE+1)*H_GRILLE -1] = 0;
}


