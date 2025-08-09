#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "srvftp.h"
#include "util.h"

// Esta variable externa es asignada en main, a través del código
// inyectado por el script en python.
// Su valor es usado por los diferentes servicios implementados aqui
extern char *documentroot;

void libera_lista(listalin *l)
{
    // Función que libera la memoria de cada nodo de la lista
    // enlazada de líneas devuelta por el servidor en modo "rodaja"
    // Será llamada desde ese servicio para liberar la memoria
    // reservada durante la ejecución previa del mismo servicio

    listalin *aux;
    while (l != NULL)
    {
        aux = l;
        l = l->sgte;
        free(aux->linea);
        free(aux);
    }
}

Resultado *isfile_1_svc(cadenaS *fname, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    static Resultado res;
    struct stat buf; // Para la llamada a stat()

    // Comprueba si el archivo documentroot/fname existe
    // y retorna apropiadamente el resultado en res
    // A RELLENAR
    |
    |
    |
    |
    |
    return (&res);
}

Resultado *isreadable_1_svc(cadenaS *fname, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    static Resultado res;

    // Comprueba si el archivo documentroot/fname existe
    // y retorna apropiadamente el resultado en res
    // A RELLENAR
    |
    |
    |
    |
    |
    return (&res);
}

Resultado *getnumlines_1_svc(cadenaS *fname, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    FILE *fp;              // Fichero cuyas líneas vamos a contar
    char linea[TAMLINEA];  // Buffer para leer una línea
    int i = 0;             // contador
    static Resultado res;

    // Abrir documentroot/fname y contar su número de líneas
    // retornando apropiadamente el resultado en res
    // A RELLENAR
    |
    |
    |
    |
    |
    |
    |
    |
    return (&res);
}

Resultado *getline_1_svc(param1 *fileitem, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    FILE *fp;              // Fichero del que vamos a leer
    char linea[TAMLINEA];  // Buffer para leer una línea
    int i = 1;             // contador
    static Resultado res;

    sprintf(filename, "%s/%s", documentroot, fileitem->filename);

    // Abrir documentroot/fname y leer la línea solicitada
    // para lo cual se leen líneas de una en una, contándolas, hasta llegar a la solicitada
    // Si se termina de leer el fichero antes de llegar a la línea solicitada
    // se informa apropiadamente al cliente
    // Se retorna apropiadamente el resultado en res

    // A RELLENAR
    |
    |
    |
    |
    |
    |
    |
    |
    |
    |

    fclose(fp);
    res.caso = 4;
    res.Resultado_u.err = "No se encontró la linea pedida en el fichero\n";
    return (&res);
}

Resultado *getlines_1_svc(param2 *fileitem, struct svc_req *peticion)
{
    char filename[TAMDIR]; // Para contener el nombre de fichero incluyendo el documentroot
    FILE *fp;              // Fichero del que vamos a leer
    char linea[TAMLINEA];  // Buffer para leer una línea
    int i = 1;             // contador
    static Resultado res;

    // Variables auxiliares para ir construyendo la lista enlazada
    static listalin *primera = NULL; // puntero al primer nodo de la lista enlazada
    listalin *ultima = NULL;  // puntero al último nodo de la lista enlazada

    // Liberar memoria de la lista de líneas que pudiera haber sido
    // reservada en una ejecución previa de este mismo servicio
    libera_lista(primera);

    // Abrir documentroot/fname
    sprintf(filename, "%s/%s", documentroot, fileitem->filename);
    fprintf(stderr, "Servidor: El cliente pide las linea %d a %d del fichero %s\n", fileitem->lineaini, fileitem->lineafin, filename);
    if ((fp = fopen(filename, "r")) == NULL)
    {
        res.caso = 4;
        res.Resultado_u.err = "Error al abrir el archivo para obtener un conjunto de lineas\n";
        return (&res);
    }
    res.caso = 3;
    res.Resultado_u.lista = NULL;

    // Para cada línea leída del fichero, se añade a la lista enlazada
    // si está en el rango solicitado. Se usará strdup() para guardar 
    // en el nodo de la lista un duplicado de la cadena leída.
    // Si se alcanzó el final del rango se sale del bucle.
    while (fgets(linea, TAMLINEA, fp) != NULL)
    {
        // A RELLENAR
        // Nota, el primer nodo de la lista se añade de forma diferente
        |
        |
        |
        |
        |
        |
        |
        |
        |
        |
        |
        |
        |
 
    }
    fclose(fp);
    res.Resultado_u.lista = primera;
    return (&res);
}
