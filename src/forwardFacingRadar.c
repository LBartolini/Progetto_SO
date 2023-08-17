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

void termHandlerFFR(int);

void mainForwardFacingRadar(){
    signal(SIGTERM, termHandlerFFR);
    logFFR = open(FFR_LOG, O_WRONLY);
    if(logFFR == -1) exit(0);

    writeLine(logFFR, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, FFR);
    writeLine(logFFR, "Connessione stabilita con successo");
}

void termHandlerFFR(int sig){
    close(sock);
    close(logFFR);
    close(fdURandom);
    exit(EXIT_SUCCESS);
}