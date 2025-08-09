# EPI (Gijon)
# Grado en Ingenieria Informatica
# Sistemas de Distribuidos (Curso 3)
# Aplicaciones Distribuidas en Linux/Unix con Sun RPC


#****************************************************
# Variables
#****************************************************


CC=gcc
RPCGEN=rpcgen
CFLAGS= -I. -Wall -ggdb -I/usr/include/tirpc
LDFLAGS= -ltirpc -lpthread
CLIENTE = cliente
SERVIDOR= servidor

todo : $(CLIENTE) $(SERVIDOR)

#**************************************************
# Generacion del cliente y del servidor
#**************************************************

$(CLIENTE) : cliente.o srvftp_clnt.o srvftp_xdr.o util.o
	$(CC) $(CFLAGS) -o $(CLIENTE) cliente.o srvftp_clnt.o srvftp_xdr.o util.o $(LDFLAGS)


$(SERVIDOR) : srvftp_svc.o srvftp_xdr.o util.o servicios.o
	$(CC) $(CFLAGS) -o $(SERVIDOR) srvftp_svc.o srvftp_xdr.o util.o servicios.o $(LDFLAGS)

srvftp_svc.o : srvftp_svc.c srvftp.h
	$(CC) $(CFLAGS) -c srvftp_svc.c 

servicios.o: servicios.c srvftp.h util.h
	$(CC) $(CFLAGS) -c servicios.c

srvftp_clnt.o : srvftp_clnt.c srvftp.h
	$(CC) $(CFLAGS) -c srvftp_clnt.c

srvftp_xdr.o : srvftp_xdr.c srvftp.h
	$(CC) $(CFLAGS) -c srvftp_xdr.c

cliente.o : cliente.c srvftp.h util.h 
	$(CC) $(CFLAGS) -c cliente.c

util.o : util.c util.h srvftp.h
	$(CC) $(CFLAGS) -c util.c

srvftp.h srvftp_svc.c: srvftp.x
	$(RPCGEN) $<
	python3 apply_srvftp_svc_patches.py

#**************************************************
# Utilidades
#**************************************************

limpia : 
	rm -f *.o	

cleanall: limpia
	rm -f srvftp_*
	rm -f srvftp.h
	rm -f cliente servidor
