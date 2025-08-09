#include <stdio.h> /* Para FILE* */
#include <errno.h> /* para perror() */
#include <stdlib.h> /* para exit() */
#include <rpc/types.h>
#include <rpc/rpc.h>
#include "tipos.h"

int main() {
  Texto cadena;
  Persona alumno;
  Resultado res;

  FILE *fichero; /* Fichero donde se escribirá */
  XDR operacion;

  fichero=fopen("Persona.dat", "w"); /* Abrir para "w"rite */
  if (fichero==NULL) { /* Comprobar errores */
	perror("Al abrir fichero");
	exit(1);
  }

  /* Inicializar variable operacion para filtros subsiguientes */
  xdrstdio_create(&operacion, fichero, XDR_ENCODE);
  alumno.nombre="Maria";
  alumno.edad=22;

  /* Escribir la variable j en el fichero, en representacion externa */
  xdr_Persona(&operacion, &alumno); /* Llamada al filtro. Codifica y guarda */
  /* Terminado, labores finales "domésticas" */
  xdr_destroy(&operacion); /* Destruir la variable operacion */
  fclose(fichero); /* Cerrar fichero */
  return 0;
}
