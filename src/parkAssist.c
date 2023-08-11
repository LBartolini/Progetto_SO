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
#include "PA.h"
#include "utils.h"

int sock, logPA;

void termHandlerPA(int);

void mainParkAssist(int mode){
    signal(SIGTERM, termHandlerPA);
    logPA = open(PA_LOG, O_WRONLY);
    if(logPA == -1) exit(0);

    writeLine(logPA, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, PA);
    writeLine(logPA, "Connessione stabilita con successo");


    termHandlerPA(0);
}

void termHandlerPA(int sig){
    close(sock);
    close(logPA);
    exit(0);
}