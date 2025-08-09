#include <stdio.h> /* Para FILE* */
#include <errno.h> /* para perror() */
#include <stdlib.h> /* para exit() */
#include <rpc/types.h>
#include <rpc/rpc.h>
int main() {
  int j = 2024; /* Dato a escribir */
  char c;
  FILE *fichero; /* Fichero donde se escribirá */
  XDR operacion;
  fichero=fopen("basicos.dat", "w"); /* Abrir para "w"rite */
  if (fichero==NULL) { /* Comprobar errores */
	perror("Al abrir fichero");
	exit(1);
  }
  /* Inicializar variable operacion para filtros subsiguientes */
  xdrstdio_create(&operacion, fichero, XDR_ENCODE);
  printf("Introduce un valor entero: ");
  scanf("%d",&j);
  getchar();
  printf("Introduce un caracter: ");
  c=getchar();

  /* Escribir la variable j en el fichero, en representacion externa */
  xdr_int(&operacion, &j); /* Llamada al filtro. Codifica y guarda */
  xdr_char(&operacion, &c);
  /* Terminado, labores finales "domésticas" */
  xdr_destroy(&operacion); /* Destruir la variable operacion */
  fclose(fichero); /* Cerrar fichero */
  return 0;
}
