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

int main(int argc, char*argv[]) {
  Texto cadena;
  Persona alumno;
  Resultado res;

  FILE *fichero; /* Fichero donde se escribirá */
  XDR operacion;

  int sock_pasivo, sock_datos;
  struct sockaddr_in d_local;
  int leidos, i;

  sock_datos = socket(PF_INET, SOCK_STREAM, 0);
  d_serv.sin_family = AF_INET;
  d_serv.sin_addr.s_addr = inet_addr("127.0.0.1");
  d_serv.sin_port = htons(atoi(argv[1]));
  connect(sock_datos, (struct sockaddr *)&d_serv, sizeof(d_serv));

  duplicado=dup(sock_datos);
  fichero=fdopen(duplicado,"w");
  xdrstdio_create(&operacion_enviar, f_enviar, XDR_DECODE);
  cadena=NULL;
  xdr_Texto(&operacion, &cadena);
  fclose(fichero);
  xdr_destroy(&operacion);
  printf("La cadena recibida fue %s\n", cadena);
  return 0;

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
