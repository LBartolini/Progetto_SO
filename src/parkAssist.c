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
#include "SVC.h"
#include "utils.h"

int sock, logPA, fdURandom, bytesRead, fdServer, pidSVC;
char buffer[8];

void termHandlerPA(int);
int executeSVC(int);

void mainParkAssist(int mode){
    signal(SIGTERM, termHandlerPA);
    logPA = open(PA_LOG, O_WRONLY);
    if(logPA == -1) exit(EXIT_FAILURE);

    pidSVC = executeSVC(mode);
    struct CompConnection surroundViewCameras;
    fdServer = initServerSocket("parkAssist");
    surroundViewCameras = connectToComponent(fdServer);

    if(mode==NORMALE){
        fdURandom = open(INPUT_NORMALE_U, O_RDONLY);
    }else if(mode==ARTIFICIALE){
        fdURandom = open(INPUT_ARTIFICIALE_U, O_RDONLY);
    }else exit(0);
    if(fdURandom == -1) exit(EXIT_FAILURE);

    writeLine(logPA, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, PA);
    writeLine(logPA, "Connessione stabilita con successo");

    while(1){
        memset(buffer, 0, sizeof buffer);
        readLine(sock, buffer);
        if(strcmp(buffer, "PARK")!=0) exit(EXIT_FAILURE);
        writeLine(surroundViewCameras.fd, "PARK");

        for(int i=0; i<30; i++){
            memset(buffer, 0, sizeof buffer);
            bytesRead = readByte(fdURandom, buffer);
            // Legge i byte da surroundViewCamera component
            memset(surroundViewCameras.buffer, 0, sizeof surroundViewCameras.buffer);
            if (readLine(surroundViewCameras.fd, surroundViewCameras.buffer) > 0){
                writeLine(sock, surroundViewCameras.buffer);
                writeLine(logPA, surroundViewCameras.buffer);
                // per debuggare commentare i writeline di bytesRead
            }
            if(bytesRead < 8) continue;
            writeLine(sock, buffer);
            writeLine(logPA, buffer);
            sleep(1);
        }

        writeLine(sock, "END PARK");
        writeLine(logPA, "END PARK");
        kill(pidSVC, SIGUSR1);
    }

    termHandlerPA(0);
}

int executeSVC(int mode){
    int pid;
    // inizializzazione surroundViewCamera
    pid = fork();
    if (pid == 0){ 
        mainSurroundViewCameras(mode);
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);

    return pid;
}

void termHandlerPA(int sig){
    close(sock);
    close(logPA);
    close(fdURandom);
    close(fdServer);
    exit(EXIT_SUCCESS);
}