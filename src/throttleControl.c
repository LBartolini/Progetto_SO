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
char buffer[100], toLog[20];

void termHandlerTC(int);

void mainThrottleControl(){
    char toAppend[1];
    signal(SIGTERM, termHandlerTC);

    logTC = open(TC_LOG, O_WRONLY);
    if(logTC == -1) exit(0);

    writeLine(logTC, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, TC);
    writeLine(logTC, "Connessione stabilita con successo");

    while(1){
        strcpy(toLog, "AUMENTO ");
        memset(buffer, 0, sizeof buffer);
        memset(toAppend, 0, sizeof toAppend);
        readLine(sock, buffer);
        int i = 11;
        while(buffer[i] != '\0'){
            sprintf(toAppend, "%c", buffer[i]);
            strcat(toLog, toAppend);
            i++;
        }

        writeLine(logTC, toLog);
    }

    termHandlerTC(0);
}

void termHandlerTC(int sig){
    close(sock);
    close(logTC);
    exit(0);
}