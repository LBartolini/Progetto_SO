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
#include "TC.h"
#include "utils.h"

int sock, logTC;

void termHandlerTC(int);

void mainThrottleControl(){
    signal(SIGTERM, termHandlerTC);

    logTC = open(TC_LOG, O_WRONLY);
    if(logTC == -1) exit(0);

    writeLine(logTC, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, TC);
    writeLine(logTC, "Connessione stabilita con successo");

    termHandlerTC(0);
}

void termHandlerTC(int sig){
    close(sock);
    close(logTC);
    exit(0);
}