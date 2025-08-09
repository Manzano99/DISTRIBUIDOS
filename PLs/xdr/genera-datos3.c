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
  int opcion;
  char c;

  FILE *fichero; /* Fichero donde se escribirá */
  XDR operacion;

  fichero=fopen("Resultado.dat", "w"); /* Abrir para "w"rite */
  if (fichero==NULL) { /* Comprobar errores */
	perror("Al abrir fichero");
	exit(1);
  }

  /* Inicializar variable operacion para filtros subsiguientes */
  xdrstdio_create(&operacion, fichero, XDR_ENCODE);

  printf("Elige una opcion:\n");
  printf("1- Entero\n");
  printf("2- Real\n");
  printf("3- Texto\n");
  printf("Tu opcion:  ");
  scanf("%d", &opcion);
  getchar();

  switch (opcion) {
    case 1:
	printf("Introduce un valor entero: ");
	scanf("%d",&res.Resultado_u.n);
	res.caso=1;
	break;
    case 2:
	printf("Introduce un valor real: ");
	scanf("%f",&res.Resultado_u.x);
	res.caso=2;
	break;
    case 3:
	printf("Introduce una cadena: ");
	res.Resultado_u.error=(char *) malloc(sizeof(char)*100);
	fgets(res.Resultado_u.error,99,stdin);
	res.caso=3;
	break;
  }

  /* Escribir la variable j en el fichero, en representacion externa */
  xdr_Resultado(&operacion, &res); /* Llamada al filtro. Codifica y guarda */
  /* Terminado, labores finales "domésticas" */
  xdr_destroy(&operacion); /* Destruir la variable operacion */
  fclose(fichero); /* Cerrar fichero */
  return 0;
}
