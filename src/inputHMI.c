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
#include "IHMI.h"
#include "utils.h"

char string[20];
int pipeECU;

void termHandler(int sig);

void mainInputHMI(int* pipe){
    pipeECU = pipe[WRITE];
    signal(SIGTERM, termHandler);

    while(1){
        printf("\n:>");
        scanf("%s", string);

        if(strcmp(string, "INIZIO")==0 || strcmp(string, "ARRESTO")==0 || strcmp(string, "PARCHEGGIO")==0){
            writeLine(pipeECU, string);
            kill(getppid(), SIGUSR1);
        }else{
            printf("Comando non valido!\n");
            sleep(1);
        }
    }
    close(pipeECU);
}

void termHandler(int sig){
    close(pipeECU);
    exit(0);
}