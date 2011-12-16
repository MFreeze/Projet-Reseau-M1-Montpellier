CC 			= gcc -c
LINK    = gcc
CFLAGS  = -Wall -pthread -lncurses -g -O2 -lglib-2.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include 
OFLAG  	= -o

ALL_OBJ_SERV = pers_sock.o dm.o controle_cam.o
ALL_OBJ_CLI  = pers_sock.o dm.o fakeclient.o

pers_sock.o: pers_sock.c pers_sock.h
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

dm.o: dm.c dm.h pers_sock.o
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

fakeclient.o: fakeclient.c pers_sock.o dm.o
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

controle_cam.o: controle_cam.c pers_sock.o dm.o
	$(CC) $(CFLAGS) $(OFLAG) $@ $<

clean:
	rm *.o
	rm *~

controle_cam: $(ALL_OBJ_SERV)
	$(LINK) $(CFLAGS) $(OFLAG) $@ $(ALL_OBJ_SERV)

fakeclient: $(ALL_OBJ_CLI)
	$(LINK) $(CFLAGS) $(OFLAG) $@ $(ALL_OBJ_CLI)

all: fakeclient controle_cam
