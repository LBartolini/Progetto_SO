#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <signal.h>

#include "definitions.h"
#include "FFR.h"
#include "utils.h"

int sock, logFFR, fdURandom, bytesRead;
char buffer[1024];

void termHandlerFFR(int);

void mainForwardFacingRadar(int argc, char **argv){
    signal(SIGTERM, termHandlerFFR);
    logFFR = open(FFR_LOG, O_WRONLY);
    if(logFFR == -1) exit(EXIT_FAILURE);

    writeLine(logFFR, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, FFR);
    writeLine(logFFR, "Connessione stabilita con successo");

    fdURandom = open(getDataSrcUrandom(argv[1]), O_RDONLY); // apre file "urandom"
    if (fdURandom == -1)  exit(EXIT_FAILURE);

    while (1){
        memset(buffer, 0, sizeof buffer);
        bytesRead = readByte(fdURandom, buffer); // Legge 8 bytes da urandom
        if (bytesRead == 8){
            if (writeLine(logFFR, buffer) == -1){
                perror("write");
                exit(EXIT_FAILURE);
            }
            writeLine(sock, buffer); // Invia i dati alla ECU
            writeLine(logFFR, buffer); // Invia i dati alla ECU
        }
        sleep(1);
    }
    termHandlerFFR(EXIT_SUCCESS);
}

void termHandlerFFR(int sig){
    close(sock);
    close(logFFR);
    close(fdURandom);
    exit(EXIT_SUCCESS);
}