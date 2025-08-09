/*
  Cliente de RPC que simula las operaciones de varios clientes del
  servidor de FTP
*/
#include <rpc/rpc.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include <pthread.h>
#include "srvftp.h"
#include "util.h"

#define TAMLINEA 256
#define TAMMSG 128
#define MAXHILOSCLIENTE 10
#define SLICE 25

// Variables globales

// IP del proceso srvftp
char *ip_srv = NULL;

// numero de clientes
int num_clientes;

// tipo de datos que recibiran los hilos cliente
struct datos_hilo
{
    unsigned char id_cliente;
    FILE *fpin;
};

typedef struct datos_hilo datos_hilo;

pthread_mutex_t m;    // mutex para serializar las invocaciones remotas
pthread_mutex_t mpet; // mutex para realizar la exclusión mutua sobre el fichero de peticiones

void *procesarDescargas(datos_hilo *p)
{
    CLIENT *cl;            // Estructura CLIENT para las RPC
    FILE *fppet;           // Fichero del que leer las peticiones a realizar
    FILE *fpout;           // Fichero al que escribir el archivo recibido
    int id_cliente;        // Numero de cliente (para mensajes de depuración)
    char buffer[TAMLINEA]; // Buffer de lectura de una línea del fichero de peticiones
    char msg[TAMLINEA];    // Buffer para mensajes de error para enviar a log_debug()
    char *token = NULL;    // Para tokenizar las líneas del fichero de peticiones
    char *loc;             // para usar con strtok_r
    char *nomfile = NULL;  // Nombre del fichero a descargar
    int numlineas;         // Número de líneas del fichero a descargar
    int linini, linfin;    // Línea inicial y final de la rodaja a descargar
    int i;                 // Índice para bucles
    param1 q1;             // Parametro para las RPC que usen el tipo param1
    param2 q2;             // Parametro para las RPC que usen el tipo param2
    listalin *l;           // Puntero a la lista de líneas devuelta por el servidor en modo "rodaja"
    Resultado *res;        // Resultado devuelto por las RPC
                           // es un puntero a una zona de memoria estática donde se
                           // encuentra el resultado de la invocación remota

    id_cliente = p->id_cliente; // Capturar el id del cliente en una variable local
    fppet = p->fpin;            // Capturar el puntero al fichero de peticiones en una variable local
    free(p);                    // Ya no necesitamos el parámetro recibido, lo liberamos

    bzero(buffer, TAMLINEA);

    pthread_mutex_lock(&mpet); // Bloquear el mutex que protege el fichero de peticiones
    while (fgets(buffer, TAMLINEA, fppet) != NULL)
    {
        fprintf(stderr, "Cliente: %d lee la peticion FTP: %s\n", id_cliente, buffer);
        pthread_mutex_unlock(&mpet); // desbloquear el mutex que protege el fichero de peticiones

        // Tokenizar la línea leida
        loc = NULL;
        token = strtok_r(buffer, " \n", &loc);
        if (token == NULL)
        {
            sprintf(msg, "Cliente: %d Error al extraer el primer token de la peticion FTP\n", id_cliente);
            log_debug(msg);
            exit(12);
        }

        // El primer token es el nombre del fichero
        nomfile = strdup(token);
        if (nomfile == NULL)
        {
            sprintf(msg, "Error: No se pudo reservar memoria para almacenar el nombre del fichero a descargar.\n");
            exit(15);
        }
        fprintf(stderr, "Cliente: %d El fichero a descargar es %s\n", id_cliente, nomfile);

        // El segundo token es el protocolo a uasar (T o U)
        token = strtok_r(NULL, "\n", &loc);

        // Inicializamos adecuadamente la estructura CLIENT según el protocolo elegido
        if (strcmp(token, "T") == 0)
        {
            fprintf(stderr, "Cliente: %d La descarga del fichero %s se realizará mediante protocolo TCP\n", id_cliente, nomfile);
            // A RELLENAR
            |
            |
        }
        else if (strcmp(token, "U") == 0)
        {
            fprintf(stderr, "Cliente: %d La descarga del fichero %s se realizará mediante protocolo UDP\n", id_cliente, nomfile);
            // A RELLENAR
            |
            |
        }
        else // Si no es T ni U se emite un error y se termina
        {
            fprintf(stderr, "Cliente: %d el protocolo para la descarga del fichero %s es desconocido\n", id_cliente, nomfile);
            exit(45);
        }

        // Comprobación de posible error en el clnt_create
        if (cl == NULL)
        {
            clnt_pcreateerror("Error en el clnt_create del hilo Cliente");
            exit(46);
        }

        // bajo la protección del mutex, se realizan todas las invocaciones remotas
        // necesarias para la descarga de este archivo para evitar condiciones de carrera
        // en el acceso a los resultados devueltos via res
        pthread_mutex_lock(&m);

        // Primero comprobar si el fichero existe
        // A RELLENAR
        |
        |

        // Si el fichero existe, se continúa, mirando si es legible
        if ---------------------- // A RELLENAR
        {
            fprintf(stderr, "Cliente: %d El fichero %s existe en el servidor.\n", id_cliente, nomfile);

            // A RELLENAR
            |
            |

            // Si el fichero es legible, se continúa mirando cuántas líneas tiene
            if --------------------  // A RELLENAR
            {
                fprintf(stderr, "Cliente: %d El fichero %s tiene permiso de lectura.\n", id_cliente, nomfile);

                // A RELLENAR
                |
                |

                // Si el número de líneas es positivo, se continúa preparándose para descargar
                if ---------------------- // A RELLENAR
                {
                    fprintf(stderr, "Cliente: %d El fichero %s tiene %d lineas.\n", id_cliente, nomfile, res->Resultado_u.val);

                    // Abrimos el fichero donde volcaremos las lineas recibidas
                    fpout = fopen(nomfile, "w");
                    if (fpout == NULL)
                    {
                        fprintf(stderr, "Cliente: %d no puede abrir el fichero de salida %s\n", id_cliente, nomfile);
                        free(nomfile);
                        continue;
                    }

                    numlineas = res->Resultado_u.val;

                    // Para muchas líneas (más de SLICE) se descargará por "rodajas"
                    if (numlineas > SLICE)
                    {
                        fprintf(stderr, "Cliente: %d El fichero %s se descarga por rodajas.\n", id_cliente, nomfile);
                        q2.filename = strdup(nomfile);

                        // Se comienza desde la línea 1 hasta la línea SLICE
                        linini = 1;
                        linfin = SLICE;
                        // Mientras queden líneas por descargar
                        while (linini <= numlineas)
                        {
                            // Se preparan los parámetros para la RPC y se invoca
                            // A RELLENAR
                            |
                            |
                            |
                            |


                            // Se itera sobre la lista recibida para volcar cada elemento
                            // a una línea del fichero de salida, hasta llegar al último (NULL)
                            l = res->Resultado_u.lista;
                            // A RELLENAR
                            |
                            |
                            |
                            |                        

                            // Se actualizan los límites de la rodaja
                            linini = -----------  // A RELLENAR
                            linfin = min(linini + SLICE - 1, numlineas);
                        }
                        free(q2.filename);
                        // Liberar la memoria de la lista recibida
                        xdr_free(xdr_Resultado, (char *)res);                        
                    }
                    else // si el número de líneas era menor a SLICE, se descarga línea a línea
                    {
                        fprintf(stderr, "Cliente: %d El fichero %s se descarga por líneas.\n", id_cliente, nomfile);

                        // Iterar por cada línea haciendo la RPC correspondiente
                        // y escribiendo a disco el resultado
                        // Si alguna RPC retorna error se informa y se sigue con la siguiente línea
                        // A RELLENAR
                        |
                        |
                        |
                        |
                        |
                        |
                        |
                    }
                    // Una vez descargado el fichero (ya sea por rodajas o por líneas), podemos cerrarlo
                    fclose(fpout);
                }
                else // si el número de líneas es 0, se informa y se sigue con el siguiente fichero
                {
                    if (res->caso == 4)
                    {
                        fprintf(stderr, "Cliente: %d recibe mensaje de error: %s\n", id_cliente, res->Resultado_u.err);
                    }
                    else
                    {
                        fprintf(stderr, "Cliente: %d El fichero %s está vacío.\n", id_cliente, nomfile);
                    }
                }
            }
            else // Si no se puede leer el fichero, o se produjo un ereror en la RPC, se informa y se sigue con el siguiente fichero
            {
                if (res->caso == 4)
                {
                    fprintf(stderr, "Cliente: %d recibe mensaje de error: %s\n", id_cliente, res->Resultado_u.err);
                }
                else
                {
                    fprintf(stderr, "Cliente: %d Intenta descargar un fichero %s que no tiene permiso de lectura.\n", id_cliente, nomfile);
                }
            }
        }
        else // Si el fichero no existe, o se produjo un error en la RPC, se informa y se sigue con el siguiente fichero
        {
            if (res->caso == 4)
            {
                fprintf(stderr, "Cliente: %d recibe mensaje de error: %s\n", id_cliente, res->Resultado_u.err);
            }
            else
            {
                fprintf(stderr, "Cliente: %d Intenta descargar un fichero %s que no existe en el servidor.\n", id_cliente, nomfile);
            }
        }

        // Terminadas todas las RPC necesarias para descargar este fichero, se libera
        // el cerrojo para dar oportunidad a otro hilo de entrar
        pthread_mutex_unlock(&m);

        // se puede liberar la estructura CLIENT y el nombre del fichero
        clnt_destroy(cl);
        if (nomfile != NULL)
        {
            free(nomfile);
        }
        bzero(buffer, TAMLINEA);

        // Ya que vamos a iniciar otra iteración, tomamos de nuevo el cerrojo
        pthread_mutex_lock(&mpet);
    }

    // Al terminar de leer el fichero de peticiones, se libera el cerrojo
    pthread_mutex_unlock(&mpet);
    return NULL;
}

int main(int argc, char *argv[])
{
    int i;         // Indice para bucles
    pthread_t *th; // array dinámico de referencias a hilos
    datos_hilo *q; // puntero a estructura que se pasará a los hilos
    FILE *fp;      // puntero al fichero de peticiones abierto

    if (argc != 4)
    {
        fprintf(stderr, "Forma de uso: %s <numero_clientes> <ip_serv_srvftp> <fich_descargas>\n", argv[0]);
        exit(1);
    }

    // Valida cada argumento y lo asigna a la variable apropiada
    if (!valida_numero(argv[1]))
    {
        fprintf(stderr, "El parametro <numero_clientes> no es un numero.\n");
        exit(2);
    }
    num_clientes = atoi(argv[1]);
    if ((num_clientes < 1) || (num_clientes > 10))
    {
        fprintf(stderr, "El parametro  <numero_clientes> debe ser >= 1 y <= 10\n");
        exit(3);
    }
    ip_srv = strdup(argv[2]);
    if (!valida_ip(argv[2]))
    {
        fprintf(stderr, "La IP introducida no es valida\n");
        exit(4);
    }

    // Intenta abrir el fichero por si hubiera problemas abortar (aunque main
    // no usa este fichero sino que se lo pasará a los hilos Cliente)
    if ((fp = fopen(argv[3], "r")) == NULL)
    {
        fprintf(stderr, "No se puede abrir el fichero con las peticiones FTP a realizar\n");
        exit(5);
    }

    // Ya que los stub de cliente comparten una variable estática, para evitar que los
    // diferentes hilos cliente se pisen entre ellos, se usa un mutex para evitar
    // hacer la misma RPC desde dos hilos a la vez
    // A RELLENAR
    |
    |
    |
    |

    // Hace falta otro mutex para evitar que dos hilos lean a la vez del
    // fichero de peticiones
    // A RELLENAR
    |
    |
    |
    |

    // Reservar espacio para el array dinámico de referencias a hilos
    th = (pthread_t *)malloc(sizeof(pthread_t) * num_clientes);
    if (th == NULL)
    {
        fprintf(stderr, "No se pudo reservar espacio para las referencias a hilos.\n");
        exit(8);
    }

    // Lanzar los hilos
    // A RELLENAR
    |
    |
    |
    |
    |
    |
    |
    |

    // Esperar a que terminen todos los hilos
    // A RELLENAR
    |
    |
    |
    |
    |
    
    fclose(fp);
    exit(0);
}
