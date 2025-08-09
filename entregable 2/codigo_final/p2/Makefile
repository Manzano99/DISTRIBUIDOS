CC = gcc
CFLAGS = -Wall -g

COMMON = protocolo.c
COMMON_OBJ = protocolo.o

all: cliente-con-servidor servidor

cliente-con-servidor: cliente-con-servidor.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o cliente-con-servidor cliente-con-servidor.o $(COMMON_OBJ)

servidor: servidor.o $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o servidor servidor.o $(COMMON_OBJ)

protocolo.o: protocolo.c protocolo.h
	$(CC) $(CFLAGS) -c protocolo.c

cliente-con-servidor.o: cliente-con-servidor.c protocolo.h
	$(CC) $(CFLAGS) -c cliente-con-servidor.c

servidor.o: servidor.c protocolo.h
	$(CC) $(CFLAGS) -c servidor.c

clean:
	rm -f *.o cliente-con-servidor servidor
