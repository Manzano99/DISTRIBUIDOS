#include <stdio.h>

void main(void)
{
  int *p=NULL;

  printf("Antes de usar el puntero sin inicializar\n");
  *p=*p*3;
  printf("Despues de usar el puntero sin inicializar\n");
}
