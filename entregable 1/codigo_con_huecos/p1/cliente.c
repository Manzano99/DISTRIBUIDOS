
// Archivos de cabecera para manipulación de sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include "protocolo.h"
#include "util.h"

//
// VARIABLES GLOBALES
//

// IP del proceso srvdns
char *ip_srvftp;

// Puerto en el que espera el proceso srvdns
int puerto_srvftp;

// Numero de hilos lectores
int nhilos;

// nombre del fichero fuente de peticiones
char *fich_peticiones;

// nombre del fichero de log
char *fich_log;

// handlers de archivo
FILE *fppet;
FILE *fplog;

// mutex de exclusión al fichero de peticiones y de log
pthread_mutex_t mutex_fppet = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_fplog = PTHREAD_MUTEX_INITIALIZER;

void procesa_argumentos(int argc, char *argv[])
{
    if (argc < 6)
    {
        fprintf(stderr, "Forma de uso: %s ip_srvftp puerto_srvftp fich_pet nhilos fich_log\n", argv[0]);
        exit(1);
    }
    ip_srvftp = strdup(argv[1]);
    if (!valida_ip(argv[1]))
    {
        fprintf(stderr, "Error: La IP introducida no es valida\n");
        exit(2);
    }
    if (!valida_numero(argv[2]))
    {
        fprintf(stderr, "Error: El valor de puerto no es un numero\n");
        exit(3);
    }
    puerto_srvftp = atoi(argv[2]);
    if ((puerto_srvftp < 1024) || (puerto_srvftp > 65535))
    {
        fprintf(stderr, "Error: El puerto debe ser mayor o igual a 1024 y menor o igual que 65535\n");
        exit(4);
    }
    fich_peticiones = argv[3];
    if (!valida_numero(argv[4]))
    {
        fprintf(stderr, "Error: El parámetro de número de hilos no es un número\n");
        exit(6);
    }
    nhilos = atoi(argv[4]);
    if (nhilos < 1)
    {
        fprintf(stderr, "Error: El numero de hilos cliente debe ser mayor o igual a 1\n");
        exit(7);
    }
    fich_log = argv[5];
}

void print_log(const char *format, ...)
{
    // Función auxiliar para imprimir mensajes en el fichero de log
    //
    // Se usa igual que printf, por lo que recibe un número variable de argumentos
    // y una cadena de formato. Por ejemplo:
    //
    // print_log("Error: No se pudo abrir el fichero %s\n", nombre_fichero);

    char buffer[TAMMSG];
    va_list args;

    // El siguiente trozo accede a la cadena de formato recibida y a la lista
    // de argumentos que vayan detrás, y le pasa todo a vsnprintf para que
    // formatee la cadena con los parámetros adecuadamente.
    // El resultado (que queda almacenado en buffer) es el mensaje que
    // queremos enviar finalmente al fichero de log
    va_start(args, format);
    vsnprintf(buffer, TAMMSG, format, args);
    va_end(args);

    // Escribimos el mensaje en buffer en el fichero fplog, protegiendo
    // el acceso con el mutex apropiado.
    // A RELLENAR
    |
    |
    |
    |
}

void salir_bien(int s)
{
    exit(0);
}

char *enviar_peticion(char *peticion, struct sockaddr_in *d_serv, int es_stream)
{
    /*
        Esta función envía un mensaje (en el parámetro peticion) al servidor, usando
        TCP o UDP según el valor del parámetro es_stream. La IP y puerto del servidor
        vienen en el parámetro d_serv.

        Una vez enviada la petición se espera a recibir una respuesta y se retorna
        dicha respuesta en un puntero a cadena, reservada dinámicamente (con strndup)

        Para las comunicaciones de red hace uso de las funciones en protocolo.c

        Quien llame a esta función debe liberar la memoria del buffer devuelto una
        vez que haya terminado de usarlo.
    */
    int sock_dat;
    int enviados, recibidos;
    char respuesta[TAMLINEA];
    char *s;

    bzero(respuesta, TAMLINEA);
    if (es_stream)
    {

        // A RELLENAR
        |
        |
        |
        |
    }
    else // se envía la petición por socket UDP
    {
        // A RELLENAR
        |
        |
        |
        |
    }
    close(sock_dat);
    s = strndup(respuesta, TAMLINEA);
    if (s == NULL)
    {
        print_log("Error al enviar peticion: No se pudo reservar memoria para hacer un duplicado de la respuesta\n");
        exit(14);
    }
    // A RELLENAR
    |
    |
}

void *hilo_descarga(struct sockaddr_in *p)
{
    register int i;          // para iterar por las líneas
    int numlineas;           // para obtener el número de líneas del fichero
    char peticion[TAMMSG];   // para construir las peticiones a enviar
    char buffer[TAMLINEA];   // para leer una línea del fichero de peticiones
    char *s;                 // para almacenar la respuesta del servidor
    FILE *fpout;             // para escribir en el fichero de salida
    char *pt = NULL;         // para separar los tokens de la línea leída
    char *loc;               // para poder usar strtok_r 
    char *nomfile = NULL;    // para almacenar el nombre del fichero
    unsigned char es_stream; // para indicar si la petición se hace por TCP o UDP

    // Coger cerrojo antes de leer una línea del fichero de peticiones
    pthread_mutex_lock(&mutex_fppet);
    fprintf(stderr, "Hilo lector antes de entrar en el bucle\n");
    while (fgets(buffer, TAMLINEA, fppet) != NULL)
    {
        // Soltarlo tras haber leido la línea
        pthread_mutex_unlock(&mutex_fppet);

        // quitamos el salto de linea
        // la linea leida es de la forma "nombre_fichero T\n" o "nombre_fichero U\n"
        buffer[strlen(buffer) - 1] = '\0';

        // Separamos en tokens, siendo posibles separadores el espacio y el salto de linea
        pt = strtok_r(buffer, " \n", &loc);
        
        // Primer token, nombre de fichero
        if (pt == NULL)
        {
            print_log("Error: No se pudo obtener el nombre del fichero de la peticion\n");
            exit(15);
        }
        nomfile = strdup(pt);
        if (nomfile == NULL)
        {
            print_log("Error: No se pudo reservar memoria para el nombre del fichero de la peticion\n");
            exit(15);
        }

        // Segundo token, "T" o "U"
        pt = strtok_r(NULL, " \n", &loc);
        // Asignar apropiadamente es_stream
        // A RELLENAR
        |
        |
        |

        fprintf(stderr, "Hilo lector antes de enviar peticion sobre fichero %s\n", nomfile);

        // Preparar peticion para ver si existe el fichero, y enviarla
        // A RELLENAR
        |
        |
        if (strcmp(s, "OK") == 0)
        {
            free(s);
            // Preparar peticion para ver si es legible, y enviarla
            // A RELLENAR
            |
            |
            if (strcmp(s, "OK") == 0)
            {
                free(s);
                // Preparar peticion para obtener el numero de lineas, y enviarla
                // A RELLENAR
                |
                |
                if (s != NULL)
                {
                    // Extraer el numero de lineas de la respuesta
                    sscanf(s, "%d", &numlineas);
                    free(s);
                    if (numlineas > 0)
                    {
                        // Abrir fichero localmente para escribir en él lo que se reciba
                        if ((fpout = fopen(buffer, "w")) == NULL)
                        {
                            print_log("Error: No se pudo abrir el fichero %s para escribir\n", nomfile);
                            exit(16); 
                        }
                        // Iterar por el número de líneas a recibir
                        for (i = 0; i < numlineas; i++)
                        {
                            // Preparar peticion para obtener la linea i, y enviarla
                            // A RELLENAR
                            |
                            |
                            // Escribir en el fichero la línea recibida
                            // A RELLENAR
                            |
                            free(s);
                        }
                        fclose(fpout);
                    }
                    else // Si numlineas es 0
                    {
                        print_log("Error: El fichero %s esta vacio\n", nomfile);
                    }
                }
                else // Manejo de error al obtener el número de líneas
                {
                    free(s);
                    print_log("Error: No se pudo obtener el numero de lineas del fichero %s\n", nomfile);
                }
            }
            else // Manejo de error al comprobar si es legible
            {
                free(s);
                print_log("Error: El fichero %s no tiene permiso de lectura\n", nomfile);
            }
        }
        else // Manejo de error al comprobar si existe
        {
            free(s);
            print_log("Error: El fichero %s no existe en el servidor\n", nomfile);
        }

        fprintf(stderr, "Hilo lector antes de hacer mutex lock de mutex_fppet\n");
        if (nomfile != NULL)
            free(nomfile);
        // Coger cerrojo para la siguiente iteración del bucle
        pthread_mutex_lock(&mutex_fppet);
    }
    // Soltarlo tras la última iteración
    pthread_mutex_unlock(&mutex_fppet);
}

void main(int argc, char *argv[])
{
    register int i;             // Para iterar en bucles
    pthread_t *th;              // array dinámico de identificadores de hilo
    struct sockaddr_in d_serv;  // Para contener IP-puerto del servidor

    signal(SIGINT, salir_bien);
    procesa_argumentos(argc, argv);

    // Se abren los ficheros de peticiones y de log
    // y las correspondientes variables globales serán usadas desde los hilos
    if ((fppet = fopen(fich_peticiones, "r")) == NULL)
    {
        perror("Error: No se pudo abrir el fichero de peticiones");
        exit(8);
    }

    if ((fplog = fopen(fich_log, "w")) == NULL)
    {
        perror("Error: No se pudo abrir el fichero de log");
        exit(9);
    }

    // Reserva dinámica de memoria para el array de identificadores de hilo
    // A RELLENAR
    |
    |
    |
    |
    

    // Configuración de la estructura con la IP y puerto del servidor
    d_serv.sin_family = AF_INET;
    d_serv.sin_addr.s_addr = inet_addr(ip_srvftp);
    d_serv.sin_port = htons(puerto_srvftp);

    // Creación de los hilos de descarga
    // A RELLENAR
    |
    |
    |
    |


    // Espera a que todos los hilos terminen
    for (i = 0; i < nhilos; i++)
    {
        pthread_join(th[i], NULL);
    }
}
