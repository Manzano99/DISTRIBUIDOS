#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h> //fichero de cabecera de la llamada la sistema signal
#include <sys/types.h>
#include <sys/wait.h>

void atrapa_SIGCHLD(int signum) {
    wait(0);
    return;
}


int main(int argc, char* argv[]) {
    int sock_pasivo, sock_datos;
    struct sockaddr_in d_local;
    char mensaje[100]; //array donde escribe el cliente
    int leidos; //numero bytes leidos socket
    int puerto;
    pid_t pid; //variable para recoger el pid y valor retornado por el fork()

    puerto=atoi(argv[1]); //guarda el primer argumento que llega por la linea de comandos

    signal(SIGCHLD, atrapa_SIGCHLD); //atrapamos la señal cada vez que se reciba

    sock_pasivo = socket(PF_INET, SOCK_STREAM, 0);

    d_local.sin_family = AF_INET;
    d_local.sin_addr.s_addr = htonl(INADDR_ANY); //cualquier IP que tenga mi maquina
    d_local.sin_port = htons(puerto);

    bind(sock_pasivo, (struct sockaddr *)&d_local, sizeof(d_local));
    listen(sock_pasivo, SOMAXCONN);

    while (1) { // Bucle infinito de atención a clientes
         printf("Soy el proceso padre (%d) antes del accept()\n", getpid());
         sock_datos = accept(sock_pasivo, 0, 0);
         //Nos clonaremos tan pronto retornemos del accept()
         printf("Soy el proceso padre (%d) antes del fork()\n", getpid());
         if ((pid=fork())<0) //Error en el fork()
         {
            perror("Fallo del fork(). El proceso padre atiende personalmente.");
         while((leidos=read(sock_datos,mensaje,sizeof(mensaje)))>0) {
             for(int i=0; i<leidos; i++) mensaje [i]= (char) toupper(mensaje[i]); 
                 write(sock_datos, mensaje, leidos);
         }
         close(sock_datos);
    	}
        else if(pid==0) //estamos en el retorno del fork() en el hijo
        {
         printf("Soy el proceso hijo (%d) antes del bucle de lectura del socket\n", getpid());
         while((leidos=read(sock_datos,mensaje,sizeof(mensaje)))>0) {
             for(int i=0; i<leidos; i++)mensaje [i]= (char) toupper(mensaje[i]);
                 write(sock_datos, mensaje, leidos);
         }
         close(sock_datos);
         printf("Soy el proceso hijo (%d) antes de terminar\n", getpid());
	 exit(0); //el subproceso hijo termina
        }
        else //retorno del fork() en el proceso padre (original)
        {
         printf("Soy el proceso padre (%d) antes de cerrar el socket de datos\n", getpid());
         close(sock_datos);
        }
    }
}
