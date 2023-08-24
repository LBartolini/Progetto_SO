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

void mainForwardFacingRadar(int mode){
    signal(SIGTERM, termHandlerFFR);
    logFFR = open(FFR_LOG, O_WRONLY);
    if(logFFR == -1) exit(EXIT_FAILURE);

    writeLine(logFFR, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, FFR);
    writeLine(logFFR, "Connessione stabilita con successo");
    
    if(mode==NORMALE){
        fdURandom = open(INPUT_NORMALE_U, O_RDONLY);
    }else if(mode==ARTIFICIALE){
        fdURandom = open(INPUT_ARTIFICIALE_U, O_RDONLY);
    }else exit(0);
    if(fdURandom == -1) exit(EXIT_FAILURE);

    while (1){
        memset(buffer, 0, sizeof buffer);
        bytesRead = readByte(fdURandom, buffer); // Legge 8 bytes da urandom
        if (bytesRead == 8){
            if (writeLine(logFFR, buffer) == -1) exit(EXIT_FAILURE);
            writeLine(sock, buffer); // Invia i dati alla ECU
            writeLine(logFFR, buffer); // Invia i dati al logger
        }
        sleep(1);
    }
    termHandlerFFR(0);
}

void termHandlerFFR(int sig){
    close(sock);
    close(logFFR);
    close(fdURandom);
    exit(EXIT_SUCCESS);
}