CC 		= gcc -g -c
LINK    = gcc -lncurses -pthread -D_REENTRANT
CFLAGS  = -Wall -O2 -D_REENTRANT -pthread
OFLAG  	= -o

CSERVENVOI=fonctionsServeurs.c fonctionsServeurE.c serveurEnvoi.c
CSERVRECEPT=fonctionsServeurs.c fonctionsServeurR.c serveurRecept.c
CCLIENT=gui.c pers_sock.c fonctionsClient.c client.c

OSERVENVOI=fonctionsServeurs.o fonctionsServeurE.o serveurEnvoi.o
OSERVRECEPT=fonctionsServeurs.o fonctionsServeurR.o serveurRecept.o
OCLIENT=gui.o pers_sock.o fonctionsClient.o client.o

ALL_OBJ_SERV = pers_sock.o dm.o controle_cam.o
ALL_OBJ_CLI  = gui.o pers_sock.o fakeclient.o

all: fakeclient controle_cam serveur serveurRecept client

pers_sock.o: pers_sock.c pers_sock.h
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

gui.o: gui.c gui.h
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

serveur : OSERVENVOI
	$(LINK) $(OSERVENVOI) $(OFLAG) $@
	
serveurRecept : OSERVRECEPT
	$(LINK) $(OSERVRECEPT) $(OFLAG) $@

client: OCLIENT
	$(LINK) $(OCLIENT) $(OFLAG) $@


