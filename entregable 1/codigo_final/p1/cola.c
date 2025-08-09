#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cola.h"

void init_cola(Cola *cola, int tam_cola)
{
    register int i;

    if (tam_cola < 1)
    {
        perror("Error: El tamaño de la cola debe ser mayor o igual que 1");
        exit(1);
    }
    if (cola == NULL)
    {
        perror("Error: El puntero a la cola es NULL en inicializar_cola");
        exit(2);
    }
    // A RELLENAR el resto de la inicialización de la cola
    cola->datos = (dato_cola **)malloc(sizeof(dato_cola *) * tam_cola);
    if (cola->datos == NULL)
    {
        // Si no se pudo reservar memoria se muestra un error y se termina
        perror("Error: No se pudo reservar memoria para la cola");
        exit(3);
    }

    // Se inicializan los índices de la cola
    cola->head = 0;
    cola->tail = 0;
    cola->tam_cola = tam_cola;

    // Se inicializan los mutex para acceder a head y tail
    pthread_mutex_init(&cola->mutex_head, NULL);
    pthread_mutex_init(&cola->mutex_tail, NULL);

    // Se inicializan los semáforos para controlar los huecos y elementos ocupados
    sem_init(&cola->num_huecos, 0, tam_cola);
    sem_init(&cola->num_ocupados, 0, 0);

}

void liberar_cola(Cola *cola)
{
    // Debe liberarse la memoria apuntada por cada puntero guardado en la cola
    // y la propia memoria donde se guardan esos punteros, así como
    // destruir los semáforos y mutexes
    
    // A RELLENAR
    // Se libera la memoria de cada elemento almacenado en la cola
    for (int i = 0; i < cola->tam_cola; i++)
    {
        if (cola->datos[i] != NULL)
        {
            free(cola->datos[i]);
        }
    }

    // Liberación de memoria del array de punteros de la cola
    free(cola->datos);

    // Destruir los mutex
    pthread_mutex_destroy(&cola->mutex_head);
    pthread_mutex_destroy(&cola->mutex_tail);

    // Destruir los semáforos
    sem_destroy(&cola->num_huecos);
    sem_destroy(&cola->num_ocupados);
}

void meter_en_cola(Cola *cola, dato_cola *dato)
{
    // copiamos el puntero sin mas en la posición adecuada de la cola
    // A RELLENAR
    sem_wait(&cola->num_huecos);
    pthread_mutex_lock(&cola->mutex_tail);

    cola->datos[cola->tail] = dato;
    cola->tail = (cola->tail + 1) % cola->tam_cola;

    pthread_mutex_unlock(&cola->mutex_tail);
    sem_post(&cola->num_ocupados);
}

dato_cola *sacar_de_cola(Cola *cola)
{
    dato_cola *p;

    // A RELLENAR
    sem_wait(&cola->num_ocupados);
    pthread_mutex_lock(&cola->mutex_head);
    p = cola->datos[cola->head];
    cola->head = (cola->head + 1) % cola->tam_cola;
    pthread_mutex_unlock(&cola->mutex_head);
    sem_post(&cola->num_huecos);
    return p;
}

