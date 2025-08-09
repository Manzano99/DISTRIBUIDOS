#include <stdio.h> /* Para FILE* */
#include <errno.h> /* para perror() */
#include <stdlib.h> /* para exit() */
#include <rpc/types.h>
#include <rpc/rpc.h>
#include "tipos.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  Texto cadena;
  Persona alumno;
  Resultado res;

  FILE *fichero; /* Fichero donde se escribirá */
  XDR operacion;

  int sock_pasivo, sock_datos;
  struct sockaddr_in d_local;
  int leidos, i;

  sock_pasivo = socket(PF_INET, SOCK_STREAM, 0);
  d_local.sin_family = AF_INET;
  d_local.sin_addr.s_addr = htonl(INADDR_ANY);
  d_local.sin_port = htons(atoi(argv[1]));
  bind(sock_pasivo, (struct sockaddr *)&d_local, sizeof(d_local));
  listen(sock_pasivo, SOMAXCONN);

  while(1){
    sock_datos = accept(sock_pasivo, 0, 0);
    duplicado=dup(sock_datos);
    fichero=fdopen(duplicado,"w");
    xdrstdio_create(&operacion_enviar, f_enviar, XDR_ENCODE);
    xdr_Texto(&operacon, &cadena);
    fclose(fichero);
    xdr_destroy(&operacion);
  }

  /* Inicializar variable operacion para filtros subsiguientes */
  xdrstdio_create(&operacion, fichero, XDR_ENCODE);
  cadena="Probando";

  /* Escribir la variable j en el fichero, en representacion externa */
  xdr_Texto(&operacion, &cadena); /* Llamada al filtro. Codifica y guarda */
  /* Terminado, labores finales "domésticas" */
  xdr_destroy(&operacion); /* Destruir la variable operacion */
  fclose(fichero); /* Cerrar fichero */
  return 0;
}
