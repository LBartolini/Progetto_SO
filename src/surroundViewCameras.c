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

int sock, logSVC, fdURandom, bytesRead, parking;
char buffer[1024];

void termHandlerSVC(int);

void mainSurroundViewCameras(int mode){
    signal(SIGTERM, termHandlerSVC);
    logSVC = open(SVC_LOG, O_WRONLY);
    if(logSVC == -1) exit(EXIT_FAILURE);

    writeLine(logSVC, "Connessione a PA");
    sock = connectToServer(PARK_ASSIST);
    writeLine(sock, SVC);
    writeLine(logSVC, "Connessione stabilita con successo");
    
    if(mode==NORMALE){
        fdURandom = open(INPUT_NORMALE_U, O_RDONLY);
    }else if(mode==ARTIFICIALE){
        fdURandom = open(INPUT_ARTIFICIALE_U, O_RDONLY);
    }else exit(0);
    if(fdURandom == -1) exit(EXIT_FAILURE);

    while(1){
        memset(buffer, 0, sizeof buffer);
        bytesRead = readByte(fdURandom, buffer); // Legge 8 bytes da urandom
        if (bytesRead < 8) continue;

        writeLine(sock, buffer); // Invia i dati a PA
        writeLine(logSVC, buffer); // Invia i dati al logger
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