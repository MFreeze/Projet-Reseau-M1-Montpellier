CC 		= gcc -c
LINK    = gcc -lglib-2.0 -lncurses -pthread -D_REENTRANT
CFLAGS  = -Wall -g -O2 -I/usr/include/glib-2.0 -D_REENTRANT -pthread
OFLAG  	= -o

CSERVENVOI=fonctionsServeurs.c serveurEnvoi.c
CSERVRECEPT=fonctionsServeurs.c serveurRecept.c
CCLIENT=fonctionsClient.c client.c

OSERVENVOI=fonctionsServeurs.o serveurEnvoi.o
OSERVRECEPT=fonctionsServeurs.o serveurRecept.o
OCLIENT=fonctionsClient.o client.o

ALL_OBJ_SERV = pers_sock.o dm.o controle_cam.o
ALL_OBJ_CLI  = pers_sock.o dm.o fakeclient.o

all: fakeclient controle_cam serveurEnvoi serveurRecept client

pers_sock.o: pers_sock.c pers_sock.h
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

dm.o: dm.c dm.h pers_sock.o
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

fakeclient.o: fakeclient.c pers_sock.o dm.o
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

controle_cam.o: controle_cam.c pers_sock.o dm.o
	$(CC) $(CFLAGS) $(OFLAG) $@ $<
	
OSERVENVOI: $(CSERVENVOI)
	$(CC) $(CFLAGS) $(CSERVENVOI)
	
OSERVRECEPT: $(CSERVRECEPT)
	$(CC) $(CFLAGS) $(CSERVRECEPT)
	
OCLIENT: $(CCLIENT)
	$(CC) $(CFLAGS) $(CCLIENT)


clean:
	rm *.o
	rm *~

controle_cam: $(ALL_OBJ_SERV)
	$(LINK) $(OFLAG) $@ $(ALL_OBJ_SERV)

fakeclient: $(ALL_OBJ_CLI)
	$(LINK $(OFLAG) $@ $(ALL_OBJ_CLI)

serveurEnvoi : OSERVENVOI
	$(LINK) $(OSERVENVOI) $(OFLAG) $@
	
serveurRecept : OSERVRECEPT
	$(LINK) $(OSERVRECEPT) $(OFLAG) $@

client: OCLIENT
	$(LINK) $(OCLIENT) $(OFLAG) $@


