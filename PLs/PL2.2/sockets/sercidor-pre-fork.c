#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    int sock_pasivo, sock_datos;
    struct sockaddr_in d_local;
    char mensaje[100]; //array donde escribe el cliente
    int leidos; //numero bytes leidos socket
    int puerto;
    pid_t pid;

    puerto=atoi(argv[1]); //guarda el primer argumento que llega por la linea de comandos

    sock_pasivo = socket(PF_INET, SOCK_STREAM, 0);
    
    d_local.sin_family = AF_INET;
    d_local.sin_addr.s_addr = htonl(INADDR_ANY); //cualquier IP que tenga mi maquina
    d_local.sin_port = htons(puerto);
    
    bind(sock_pasivo, (struct sockaddr *)&d_local, sizeof(d_local));
    listen(sock_pasivo, SOMAXCONN);
    //aqui creariamos el pool de procesos servidor
    for(int i=0; i<3; i++) {
       if((pid=fork())<0) {  //fallo del fork()
           perror("Fallo del fork() al generar un subproceso del pool");
           continue;
       }
       else if(pid==0) { //retorno del fork() en el hijo
           break;
       }
       else //retorno del fork en el padre(no hace falta)
       {
	 continue;
       }
    }

    while (1) { // Bucle infinito de atención a clientes
         printf("Soy el proceso (%d) antes del accept\n", getpid());
         sock_datos = accept(sock_pasivo, 0, 0);
         printf("Soy el proceso (%d) atendiendo a un cliente\n", getpid());
         printf("Soy el proceso (%d) antes del bucle de lectura del socket\n", getpid());
         while((leidos=read(sock_datos,mensaje,sizeof(mensaje)))>0) {
             for(int i=0; i<leidos; i++) mensaje[i] = (char) toupper(mensaje[i]); 
             write(sock_datos, mensaje, leidos);
         }
         printf("Soy el proceso (%d) antes de cerrar el socket\n", getpid());
         close(sock_datos);
    }
}
