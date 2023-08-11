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
#include "FWC.h"
#include "utils.h"

int sock, logFWC;

void termHandlerFWC(int);

void mainFrontWindshieldCamera(){
    signal(SIGTERM, termHandlerFWC);
    
    logFWC = open(FWC_LOG, O_WRONLY);
    if(logFWC == -1) exit(0);

    writeLine(logFWC, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, FWC);
    writeLine(logFWC, "Connessione stabilita con successo");

    termHandlerFWC(0);
}

void termHandlerFWC(int sig){
    close(sock);
    close(logFWC);
    exit(0);
}