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
#include "SVC.h"
#include "utils.h"

int sock, logSVC, fdURandom, bytesRead;
char buffer[1024];

void termHandlerSVC(int);

void mainSurroundViewCameras(int argc){
    signal(SIGTERM, termHandlerSVC);
    logSVC = open(SVC_LOG, O_WRONLY);
    if(logSVC == -1) exit(EXIT_FAILURE);

    writeLine(logSVC, "Connessione alla ECU");
    sock = connectToServer(PARK_ASSIST);
    writeLine(sock, SVC);
    writeLine(logSVC, "Connessione stabilita con successo");
    fdURandom = open(INPUT_NORMALE_U, O_RDONLY); // apre file "urandom"
    if (fdURandom == -1)  exit(EXIT_FAILURE);
    while (1){
        memset(buffer, 0, sizeof buffer);
        bytesRead = readByte(fdURandom, buffer); // Legge 8 bytes da urandom
        if (bytesRead == 8){
            if (writeLine(logSVC, buffer) == -1) exit(EXIT_FAILURE);
            writeLine(sock, buffer); // Invia i dati alla ECU
            writeLine(logSVC, buffer); // Invia i dati al logger
        }
        sleep(1);
    }
    termHandlerSVC(0);
}

void termHandlerSVC(int sig){
    close(sock);
    close(logSVC);
    close(fdURandom);
    exit(EXIT_SUCCESS);
}