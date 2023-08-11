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
#include "BBW.h"
#include "utils.h"

int sock, logBBW;

void termHandlerBBW(int);

void mainBrakeByWire(){
    signal(SIGTERM, termHandlerBBW);
    
    logBBW = open(BBW_LOG, O_WRONLY);
    if(logBBW == -1) exit(0);

    writeLine(logBBW, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, BBW);
    writeLine(logBBW, "Connessione stabilita con successo");

    
    termHandlerBBW(0);
}

void termHandlerBBW(int sig){
    close(sock);
    close(logBBW);
    exit(0);
}