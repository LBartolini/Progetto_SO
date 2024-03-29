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
#include <time.h>

#include "definitions.h"
#include "BBW.h"
#include "utils.h"

int sock, logBBW;
time_t timer;
struct tm* tm_info;
char buffer[60], toLog[100];

void termHandlerBBW(int);
void stopHandler(int);

void mainBrakeByWire(){
    signal(SIGTERM, termHandlerBBW);
    signal(SIGUSR1, stopHandler);
    
    logBBW = open(BBW_LOG, O_WRONLY);
    if(logBBW == -1) exit(EXIT_FAILURE);

    writeLine(logBBW, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, BBW);
    writeLine(logBBW, "Connessione stabilita con successo");

    while(1){
        memset(buffer, 0, sizeof buffer);
        memset(toLog, 0, sizeof toLog);

        readLine(sock, buffer);

        timer = time(NULL);
        tm_info = localtime(&timer);
        strftime(toLog, 26, "%Y-%m-%d %H:%M:%S - ", tm_info);
        strcat(toLog, buffer);
        writeLine(logBBW, toLog);
    }
    
    termHandlerBBW(0);
}

void termHandlerBBW(int sig){
    close(sock);
    close(logBBW);
    exit(EXIT_SUCCESS);
}

void stopHandler(int sig){
    writeLine(logBBW, "ARRESTO AUTO");
}