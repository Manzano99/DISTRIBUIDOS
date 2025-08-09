CC=gcc
CFLAGS=-I. -pthread -g
DEPS = cola.c util.c protocolo.c
OBJ = srvftp.o cola.o cliente.o util.o protocolo.o

all: srvftp cliente 

srvftp: srvftp.c $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) 

cliente: cliente.c util.c util.h protocolo.c protocolo.h
	$(CC) -o $@ $^ $(CFLAGS)  

limpia: 
	rm *.o
cleanall:
	-rm srvftp cliente *.o
	-rm *.log