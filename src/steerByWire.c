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
#include "SBW.h"
#include "utils.h"

#define NO_ACTION "NO ACTION"
#define LEFT "STO GIRANDO A SINISTRA"
#define RIGHT "STO GIRANDO A DESTRA"

int sock, logSBW;
char buffer[1024];

void termHandlerSBW(int);

void mainSteerByWire(){
    char action[32];
    signal(SIGTERM, termHandlerSBW);
    
    logSBW = open(SBW_LOG, O_WRONLY);
    if(logSBW == -1) exit(0);

    writeLine(logSBW, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, SBW);
    int f = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, f | O_NONBLOCK); 
    writeLine(logSBW, "Connessione stabilita con successo");

    while(1){
        strcpy(action, NO_ACTION);
        memset(buffer, 0, sizeof buffer);
        readLine(sock, buffer);

        if(strcmp(buffer, "SINISTRA")==0){
            strcpy(action, LEFT);
        }else if(strcmp(buffer, "DESTRA")==0){
            strcpy(action, RIGHT);
        }

        for(int i=0; i<4; i++){
            writeLine(logSBW, action);
            sleep(1);
        }
    }

    termHandlerSBW(0);
}

void termHandlerSBW(int sig){
    close(sock);
    close(logSBW);
    exit(0);
}