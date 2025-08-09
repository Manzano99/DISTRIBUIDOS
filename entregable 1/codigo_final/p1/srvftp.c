#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <semaphore.h>

// Archivos de cabecera para manipulación de sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cola.h"
#include "util.h"
#include "protocolo.h"

// ====================================================================
// PROTOTIPOS FUNCIONES
// ====================================================================
static void handler(int signum);

// ====================================================================
// VARIABLES GLOBALES
// ====================================================================

// Cola donde se introducen las consultas que van llegando al servidor FTP
Cola cola_peticiones;

// Puerto en el que esperamos los mensajes
int puerto;

// Variable que almacena el numero de hilos trabajadores
int num_hilos_work;

// Puntero a la dirección de comienzo del array de datos de hilo
// de los hilos trabajadores
pthread_t *hilos_work;

// Tamanio de la cola circular
int tam_cola;

// Variable global que almacena el nombre de la carpeta base de los ficheros
// servidos
char *documentroot = NULL;

// Semaforo de cuenta del número de descriptores usados
sem_t semdesc;

// ====================================================================
// FUNCION handler de las señales recibidas por el proceso
// ====================================================================
static void handler(int signum)
{
    register int i;

    switch (signum)
    {
    case SIGINT:
        // Finalización. Destruir semaforo, cola y liberar memoria
        liberar_cola(&cola_peticiones);
        free(hilos_work);
        sem_destroy(&semdesc);
        exit(0);
    default:
        pthread_exit(NULL);
    }
}

// Función que devuelve cierto si el nombre de una carpeta que se le pasa
// como argumento existe y es un directorio
unsigned char existe_carpeta(const char *nombre)
{
    struct stat buffer;
    return (stat(nombre, &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

// función que comprueba si un fichero existe
unsigned char file_exists(const char *filename)
{

    struct stat buffer;
    return stat(filename, &buffer) == 0 ? CIERTO : FALSO;
}

// función que comprueba si un fichero tiene permiso de lectura
unsigned char file_is_readable(const char *filename)
{
    return access(filename, R_OK) == 0;
}

void procesa_argumentos(int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Forma de uso: %s puerto tam_cola num_hilos_worker ruta_absoluta_document_root \n", argv[0]);
        exit(1);
    }
    if (!valida_numero(argv[1]))
    {
        fprintf(stderr, "Error: El puerto no es un número.\n");
        exit(3);
    }
    puerto = atoi(argv[1]);
    if ((puerto < 1024) || (puerto > 65535))
    {
        fprintf(stderr, "Error: El puerto debe ser mayor o igual a 1024 y menor o igual que 65535\n");
        exit(4);
    }
    // Validar y guardar el parámetro con el tamaño de la cola
    // A RELLENAR
    if (!valida_numero(argv[2]))
    {
        fprintf(stderr, "Error: El tamaño de la cola no es un número.\n");
        exit(5);
    }
    tam_cola = atoi(argv[2]);
    if (tam_cola < 1)
    {
        fprintf(stderr, "Error: El tamaño de la cola debe ser >= 1\n");
        exit(6);
    }

    // Validar y guardar el parámetro con el número de trabajadores
    // A RELLENAR
    if (!valida_numero(argv[3]))
    {
        fprintf(stderr, "Error: El número de hilos no es un número\n");
        exit(7);
    }
    num_hilos_work = atoi(argv[3]);
    if (num_hilos_work < 1)
    {
        fprintf(stderr, "Error: El número de hilos debe ser >= 1\n");
        exit(8);
    }

    documentroot = strdup(argv[4]);
    if (documentroot == NULL)
    {
        fprintf(stderr, "Error: No se pudo reservar memoria para almacenar el nombre de la carpeta document root\n");
        exit(12);
    }
    if (documentroot[strlen(documentroot) - 1] == '/')
        documentroot[strlen(documentroot) - 1] = 0;
    if (!existe_carpeta(documentroot))
    {
        fprintf(stderr, "Error: La carpeta %s no existe o no es una carpeta\n", documentroot);
        exit(13);
    }
}

// función que devuelve el número de líneas de un fichero
int get_num_lines(const char *filename)
{
    FILE *fp;
    char linea[TAMLINEA];
    int i = 0;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return -1;
    }
    while (fgets(linea, TAMLINEA, fp) != NULL)
        i++;
    fclose(fp);
    return i;
}

char *get_line(const char *filename, int line)
{
    FILE *fp;
    char linea[TAMLINEA];
    int i = 0;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return NULL;
    }
    while (fgets(linea, TAMLINEA, fp) != NULL)
    {
        if (i == line)
        {
            fclose(fp);
            return strdup(linea);
        }
        i++;
    }
    fclose(fp);
}

// ====================================================================
// Implementación de los hilos
// ====================================================================

void *Worker(int *id)
{
    int id_worker;               // Identificador del hilo trabajador
    dato_cola *pet;              // Dato a obtener de la cola
    char *token;                 // Para tokenizar el mensaje recibido
    char *loc;                   // parte de la tokenización
    char pantalla[TAMLINEA];     // Buffer para mensajes de depuración
    char msg[TAMLINEA];          // Buffer para mensajes a enviar
    char *cmd;                   // comando extraido del mensaje recibido
    char *argumento;             // argumento extraido del mensaje recibido
    char *aux;                   // Auxiliar para almacenar la línea leida
    char filepath[TAMDIR];       // para almacenar la ruta del fichero, incluyendo documentroot
    int num;                     // para almacenar valores enteros

    id_worker = *id;

    // Una vez guardado el id, liberamos la memoria reservada para el identificador del
    // hilo trabajador
    free(id);

    // Información de depuración
    sprintf(pantalla, "Comienza el Worker %d\n", id_worker);
    log_debug(pantalla);

    // Codigo del worker, bucle infinito
    while (1)
    {
        pet = sacar_de_cola(&cola_peticiones);
        sprintf(pantalla, "Worker %d: Extraida petición: %s\n", id_worker, pet->msg);
        log_debug(pantalla);

        // Empezamos a tokenizar el mensaje recibido
        loc = NULL;
        token = strtok_r(pet->msg, " \n\r", &loc); // A RELLENAR

        if (token == NULL)
        {
            fprintf(stderr, "Error: Mensaje mal formado\n");
            free(pet);
            continue;
        }
        // Primer token, es el comando
        cmd = strdup(token);

        // Siguiente token el primer argumento
        token = strtok_r(NULL, " \n\r", &loc);
        argumento = strdup(token);
        bzero(filepath, TAMDIR);

        // Tomar diferente acción según el comando recibido
        // En cada caso se prepara una respuesta en msg, pero
        // no se envía todavía
        if (strcmp(cmd, "ISFILE") == 0)
        {
            // Construir ruta al fichero (documentroot/argumento)
            // comprobar si existe, y preparar mensaje de respuesta
            // A RELLENAR
            sprintf(filepath, "%s/%s", documentroot, argumento);
            if (file_exists(filepath))
                strcpy(msg, "OK");
            else
                strcpy(msg, "ERROR");
        }
        else if (strcmp(cmd, "ISREADABLE") == 0)
        {
            // Construir ruta al fichero (documentroot/argumento)
            // comprobar si es legible, y preparar mensaje de respuesta            
            // A RELLENAR
            sprintf(filepath, "%s/%s", documentroot, argumento);
            if (file_is_readable(filepath))
                strcpy(msg, "OK");
            else
                strcpy(msg, "ERROR");
        }
        else if (strcmp(cmd, "GETNUMLINES") == 0)
        {
            // Construir ruta al fichero (documentroot/argumento)
            // obtener su número de líneas, y preparar mensaje de respuesta              
            sprintf(filepath, "%s/%s", documentroot, argumento);
            sem_wait(&semdesc);
            num = get_num_lines(filepath);
            sem_post(&semdesc);
            fprintf(stderr, "El fichero %s tiene %d lineas\n", filepath, num);
            sprintf(msg, "%d", num);
        }
        else if (strcmp(cmd, "GETLINE") == 0)
        {
            num = atoi(argumento);
            // el siguiente token es el nombre del fichero
            token = strtok_r(NULL, " \n\r", &loc);

            // Construir ruta al fichero (documentroot/argumento)
            // leer la línea en cuestión, sobre la variable aux
            // y copiarla al mensaje de respuesta
            // A RELLENAR
            sprintf(filepath, "%s/%s", documentroot, token);
            sem_wait(&semdesc);
            aux = get_line(filepath, num);
            sem_post(&semdesc);
            if (aux != NULL)
                strncpy(msg, aux, TAMLINEA);
            else
                strcpy(msg, "");

            // Liberar la memoria usada al leer la línea
            if (aux != NULL)
                free(aux);
        }

        // El mensaje que habíamos preparado, lo enviamos por el
        // socket apropiado, con las funciones apropiadas, según
        // el valor de pet->es_stream
        if (pet->es_stream)
        {
            // Se envía por el socket TCP y luego se cierra, pues se ha
            // terminado con este cliente
            // (lo que libera un descriptor a tener en cuenta en el semáforo)
            // A RELLENAR
            sendStringTCP(pet->s, msg);
            CerrarSocket(pet->s);
            sem_post(&semdesc);
        }
        else // es  una peticion por UDP
        {
            // Se envía por un socket UDP, que no se cierra, porque el mismo
            // será usado para otros clientes UDP.
            // A RELLENAR
            sendStringUDP(pet->s, msg, &pet->d_cliente);
        }
        free(pet);
    }
}

void RecepcionPeticiones(int sockTCP, int sockUDP)
{
    int recibidos;                          // Número de bytes recibidos
    struct sockaddr_in d_cliente;           // Dirección del cliente
    socklen_t l_dir = sizeof(d_cliente);    // Longitud de la dirección
    char pantalla[TAMMSG];                  // Buffer para mensajes de depuración
    char bufferTCP[TAMMSG];                 // Buffer para mensajes recibidos por TCP
    char bufferUDP[TAMMSG];                 // Buffer para mensajes recibidos por UDP
    dato_cola *p;                           // Puntero a la estructura de datos de la cola
    int s;                                  // Descriptor del socket obtenido en accept
    fd_set fds;                             // Conjunto de descriptores para select
    int maximo;                             // Número máximo de descriptores para select

    // Información de depuración
    sprintf(pantalla, "Comienza el Hilo de Recepcion de Peticiones \n");
    log_debug(pantalla);

    maximo = max(sockTCP, sockUDP);
    while (1) // Bucle infinito de atencion de mensajes
    {
        // Preparar fds y llamar a select
        // A RELLENAR
        FD_ZERO(&fds);
        FD_SET(sockTCP, &fds);
        FD_SET(sockUDP, &fds);
        select(maximo + 1, &fds, NULL, NULL, NULL);


        // Comprobar si hay clientes en el socket TCP
        if (FD_ISSET(sockTCP, &fds))
        {
            bzero(bufferTCP, TAMMSG);

            // Al aceptar la petición estaremos consumiendo un descriptor
            // de lo que debemos llevar la cuenta en el semáforo apropiado
            // A RELLENAR
            sem_wait(&semdesc);
            s = AceptarConexion(sockTCP);

            // Recibimos el mensaje
            recibidos = recibirStringTCP(s, bufferTCP, sizeof(bufferTCP));

            // Creamos una estructura dato_cola
            p = (dato_cola *)malloc(sizeof(dato_cola));
            if (p == NULL)
            {
                fprintf(stderr, "No se pudo reservar memoria para una nueva peticion\n");
                exit(15);
            }
            else
            {
                // Rellenamos esa estructura, acorde a la petición recibida
                // y la metemos en la cola
                // A RELLENAR
                p->s = s;
                strncpy(p->msg, bufferTCP, TAMMSG);
                p->es_stream = CIERTO;
                meter_en_cola(&cola_peticiones, p);
            }
        }

        // Comprobar si hay datos en el socket UDP
        if (FD_ISSET(sockUDP, &fds))
        {
            // fprintf(stderr, "Recibida petición UDP\n");
            bzero(&d_cliente, sizeof(d_cliente));
            bzero(bufferUDP, TAMMSG);

            // Recimos datos via UDP (esto no consume un descriptor
            // nuevo, pues el socket UDP ya estaba creado y es siempre el mismo)
            recibidos = recibirStringUDP(sockUDP, bufferUDP, sizeof(bufferUDP), &d_cliente);

            // Creamos una estructura dato_cola
            p = (dato_cola *)malloc(sizeof(dato_cola));
            if (p == NULL)
            {
                fprintf(stderr, "No se pudo reservar memoria para una nueva peticion\n");
                exit(15);
            }
            else
            {
                // Rellenamos esa estructura, acorde a la petición recibida
                // y la metemos en la cola                
                // A RELLENAR
                p->s = sockUDP;
                strncpy(p->msg, bufferUDP, TAMMSG);
                p->es_stream = FALSO;
                p->d_cliente = d_cliente;
                meter_en_cola(&cola_peticiones, p);
            }
        }
    }
}

// ====================================================================
// PROGRAMA PRINCIPAL
// ====================================================================

// Su misión es crear e inicializar los recursos de sincronización globales,
// lanzar todos los hilos

int main(int argc, char *argv[])
{
    register int i;         // Indice para bucles
    int *id;                // Variable dinámica, para pasar el id del hilo    
    int sock_pasivo_TCP;    // Socket pasivo TCP
    int sock_UDP;           // Socket UDP
    long num_max_desc;      // numero máximo de descriptores

    procesa_argumentos(argc, argv);
    setbuf(stdout, NULL); // quitamos el buffer de la salida estandar
    signal(SIGINT, handler); // establecemos el comportamiento ante la llegada asíncrona de la señal

    // Inicializar sockets TCP y UDP
    // A RELLENAR
    sock_pasivo_TCP = CrearSocketServidorTCP(puerto);
    sock_UDP = CrearSocketUDP(puerto);

    // inicializamos la cola
    init_cola(&cola_peticiones, tam_cola);

    // Obtenemos el número máximo de descriptores que pueden estar abiertos por usuario
    num_max_desc = sysconf(_SC_OPEN_MAX);

    // inicializamos el semaforo que lleva la cuenta de los descriptores usados
    // Como el usuario puede tener otros ficheros abiertos en otros proceso
    // dividimos el valor obtenido con sysconf entre 2 y le
    // descontamos la entrada estándar, salida estándar, salida
    // de error, el socket pasivo TCP y el socket UDP

    // A RELLENAR
    sem_init(&semdesc, 0, (num_max_desc / 2) - 5);

    // reservamos memoria para el array de manejadores de hilos (variable global)
    hilos_work = (pthread_t *)malloc(sizeof(pthread_t) * num_hilos_work);
    if (hilos_work == NULL)
    {
        fprintf(stderr, "ERROR: No se pudo reservar memoria para los objetos de datos de hilo trabajadores\n");
        exit(14);
    }

    // Crear todos los hilos worker, pasándole a cada uno un identificador
    // diferente a través de la variable dináimca id
    for (i = 0; i < num_hilos_work; i++)
    {
        // A RELLENAR
        id = (int *)malloc(sizeof(int));
        *id = i;
        if (pthread_create(&hilos_work[i], NULL, (void *)Worker, (void *)id) != 0)
        {
        fprintf(stderr, "ERROR: No se pudo crear el hilo trabajador %d\n", i);
        exit(15);
        }
    }

    // Invocar el bucle de recepción de peticiones
    RecepcionPeticiones(sock_pasivo_TCP, sock_UDP);

    // Del bucle anterior realmente no se retorna, por lo que el código
    // siguiente no llegará a ejecutarse. Sería de todas formas la
    // espera a que terminen todos los hilos
    for (i = 0; i < num_hilos_work; i++)
    {
        pthread_join(hilos_work[i], NULL);
    }
}
