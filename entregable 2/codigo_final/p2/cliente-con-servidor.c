#include "protocolo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <signal.h>

#define MAX_TAM_LINEA 500
#define MAX_TAM_MENSAJE 500
#define MAX_TAM_NICK 10

char *nick;
char *ip_servidor;
int puerto_servidor;
struct sockaddr_in dir_destino;
struct sockaddr_in dir_udp;
int destino_asignado = 0;

void recibir_y_mostrar_mensaje(int sock_udp) {
    char buffer[MAX_TAM_MENSAJE];
    int recibidos = recvfrom(sock_udp, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
    if (recibidos <= 0) return;
    buffer[recibidos] = 0;
    printf("\n**%s\n", buffer);
}

void leer_y_procesar_teclado(int sock_udp, char *ip_servidor, int puerto_servidor) {
    char linea[MAX_TAM_LINEA];
    char cmd[MAX_TAM_LINEA];
    int i;

    fgets(linea, sizeof(linea), stdin);
    for (i = strlen(linea) - 1; i >= 0; i--) {
        if (isspace(linea[i])) linea[i] = 0;
        else break;
    }

    if (strlen(linea) == 0) return;

    if (linea[0] == '/') {
        sscanf(linea, "%s", cmd);

        if (strcmp(cmd, "/CHAT") == 0) {
            char nick_dest[MAX_TAM_NICK];
            if (sscanf(linea, "/CHAT %s", nick_dest) != 1) {
                printf("Uso: /CHAT <nick>\n");
                return;
            }

            int s = CrearSocketDatosTCP();
            if (Conectar(s, ip_servidor, puerto_servidor) == -1) {
                perror("Conectar TCP");
                close(s);
                return;
            }

            sendByte(s, QUERY_CMD);
            sendShort(s, strlen(nick_dest));
            sendString(s, nick_dest);

            char resp = readByte(s);
            if (resp != CMD_OK) {
                printf("ERROR: el nick no está registrado en el servidor\n");
                close(s);
                return;
            }

            readEndpoint(s, &dir_destino);
            destino_asignado = 1;
            printf("OK, %s está conectado en %s:%d\n", nick_dest,
                   inet_ntoa(dir_destino.sin_addr), ntohs(dir_destino.sin_port));
            close(s);
        }

        // AÑADIDO: Pelayo Iglesias Manzano
        else if (strcmp(cmd, "/BROADCAST") == 0) {
            char *msg = strchr(linea, ' ');
            if (msg == NULL || strlen(msg) <= 1) {
                printf("Uso: /BROADCAST <mensaje>\n");
                return;
            }
            msg++;

            int s = CrearSocketDatosTCP();
            if (Conectar(s, ip_servidor, puerto_servidor) == -1) {
                perror("Conectar TCP");
                close(s);
                return;
            }

            sendByte(s, QUERY_CMD);
            sendShort(s, 1);
            sendString(s, "\n");

            char resp = readByte(s);
            if (resp != CMD_OK) {
                printf("ERROR al hacer broadcast\n");
                close(s);
                return;
            }

            short int total = readShort(s);
            fflush(stdout);
            char *mensaje = malloc(strlen(nick) + strlen(msg) + 20);
            sprintf(mensaje, "%s (BROADCAST)> %s", nick, msg);

            for (int i = 0; i < total; i++) {
                struct sockaddr_in destino;
                readEndpoint(s, &destino);
                fflush(stdout);

                if (ntohs(destino.sin_port) == ntohs(dir_udp.sin_port) &&
                    destino.sin_addr.s_addr == dir_udp.sin_addr.s_addr) {
                    continue;
                }

                sendto(sock_udp, mensaje, strlen(mensaje), 0,
                       (struct sockaddr *)&destino, sizeof(destino));
            }

            free(mensaje);
            close(s);
        }

        else {
            printf("Comando no reconocido.\n");
        }

    } else {
        if (!destino_asignado) {
            printf("ERROR: debes hacer /CHAT <nick> antes de enviar mensajes.\n");
            return;
        }

        char *mensaje = malloc(strlen(nick) + strlen(linea) + 3);
        sprintf(mensaje, "%s: %s", nick, linea);
        int enviados = sendto(sock_udp, mensaje, strlen(mensaje), 0,
                              (struct sockaddr *)&dir_destino, sizeof(dir_destino));
        if (enviados == -1) perror("Error al enviar");
        free(mensaje);
    }
}

// AÑADIDO: Pelayo Iglesias Manzano
void manejar_salida(int sig) {
    printf("\n[Cerrando cliente... desregistrando en el servidor]\n");

    int s = CrearSocketDatosTCP();
    if (Conectar(s, ip_servidor, puerto_servidor) == -1) {
        perror("Conectar TCP");
        exit(1);
    }

    sendByte(s, LEAVE_CMD);
    sendShort(s, strlen(nick));
    sendString(s, nick);

    char respuesta = readByte(s);
    if (respuesta == CMD_OK) {
        printf("Desregistrado correctamente.\n");
    } else {
        printf("No se pudo desregistrar.\n");
    }

    close(s);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sock_udp, sock_tcp;
    socklen_t len = sizeof(dir_udp);

    // AÑADIDO: Pelayo Iglesias Manzano - puerto 11111 por defecto si no se indica en la ejecución
    if (argc < 3 || argc > 4) {
        printf("Uso: %s nick ip_servidor [puerto_servidor]\n", argv[0]);
        exit(1);
    }

    nick = argv[1];
    ip_servidor = argv[2];
    puerto_servidor = (argc == 4) ? atoi(argv[3]) : 11111;

    // Captura de Ctrl+C
    signal(SIGINT, manejar_salida);

    sock_udp = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock_udp == -1) {
        perror("socket UDP");
        exit(1);
    }

    memset(&dir_udp, 0, sizeof(dir_udp));
    dir_udp.sin_family = AF_INET;
    dir_udp.sin_addr.s_addr = htonl(INADDR_ANY);
    dir_udp.sin_port = 0;

    if (bind(sock_udp, (struct sockaddr *)&dir_udp, sizeof(dir_udp)) == -1) {
        perror("bind UDP");
        exit(1);
    }

    getsockname(sock_udp, (struct sockaddr *)&dir_udp, &len);

    sock_tcp = CrearSocketDatosTCP();
    Conectar(sock_tcp, ip_servidor, puerto_servidor);

    sendByte(sock_tcp, JOIN_CMD);
    sendShort(sock_tcp, strlen(nick));
    sendString(sock_tcp, nick);
    sendShort(sock_tcp, ntohs(dir_udp.sin_port));

    char respuesta = readByte(sock_tcp);
    if (respuesta != CMD_OK) {
        printf("Error al registrar el nick\n");
        exit(1);
    }
    close(sock_tcp);

    // AÑADIDO: Pelayo Iglesias Manzano
    int s = CrearSocketDatosTCP();
    if (Conectar(s, ip_servidor, puerto_servidor) != -1) {
        sendByte(s, QUERY_CMD);
        sendShort(s, strlen(nick));
        sendString(s, nick);
        readByte(s);
        close(s);
    }

    printf("Usa el comando /CHAT <nick> para indicar el destino de tus mensajes\n");
    printf("En cualquier momento puedes poner /CHAT <nick> de nuevo\n");

    int teclado = 0;
    fd_set lectura;
    int maxfd = (teclado > sock_udp) ? teclado : sock_udp;

    printf("%s> ", nick);
    fflush(stdout);

    while (1) {
        FD_ZERO(&lectura);
        FD_SET(teclado, &lectura);
        FD_SET(sock_udp, &lectura);

        if (select(maxfd + 1, &lectura, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(sock_udp, &lectura)) {
            recibir_y_mostrar_mensaje(sock_udp);
        }

        if (FD_ISSET(teclado, &lectura)) {
            leer_y_procesar_teclado(sock_udp, ip_servidor, puerto_servidor);
        }

        printf("%s> ", nick);
        fflush(stdout);
    }
}