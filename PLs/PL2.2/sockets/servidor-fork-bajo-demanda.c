#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

// codigo de la diapositiva 11 del tema 2

int main(int argc, char* argv[]) {
  int sock_pasivo, sock_datos;
  struct sockaddr_in d_local;
  char buffer[200];
  int leidos, i;
  pid_t pid;

  sock_pasivo = socket(PF_INET, SOCK_STREAM, 0);
  d_local.sin_family = AF_INET;
  d_local.sin_addr.s_addr = htonl(INADDR_ANY);
  d_local.sin_port = htons(7890);
  bind(sock_pasivo, (struct sockaddr *)&d_local, sizeof(d_local));
  listen(sock_pasivo, SOMAXCONN);
  while (1) { // Bucle infinito de atención a clientes
	printf("Soy el proceso padre (%d) antes del accept \n", getpid());
	sock_datos = accept(sock_pasivo, 0, 0);
	printf("Soy el proceso padre(%d) antes del fork\n", getpid());
	if ((pid=fork())<0)
	{
	  perror("Fallo del fork() \n");
	}
	else if (pid==0)  //retorno del fork en el subproceso hijo
	{
	  printf("Soy el proceso hijo (%d) antes del while\n", getpid());
	  while ((leidos=read(sock_datos,buffer,sizeof(buffer)))>0){
		printf("Soy el proceso hijo (%d) antes del fork\n", getpid());
		for (i=0;i<leidos;i++) buffer[i]=toupper(buffer[i]);
		write(sock_datos,buffer,leidos);
	}
	  close(sock_datos);
	  exit(0);
	}
	else
	{
	  close(sock_datos);
	  exit(0);
	}
  }
}
