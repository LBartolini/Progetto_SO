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
#include "inputHMI.h"

int velocita;
int fdECULog;
int mod;
int pipeInputHMI[2];

int main(int argc, char *argv[])
{
    /*
    if(strcmp(argv[1], "NORMALE")==0){
        mod=NORMALE;
    }else if(strcmp(argv[1], "ARTIFICIALE")==0){
        mod=ARTIFICIALE;
    }
    */

    pipe(pipeInputHMI);
    if (fork() == 0){ // inizializzazione HMI Input
        close(pipeInputHMI[READ]);
        mainInputHMI(pipeInputHMI);
    }
    close(pipeInputHMI[WRITE]);
    char message[6];
    for(int i=0, bytesRead; i<3; i++){
        memset(message, 0, sizeof message);
        bytesRead = read(pipeInputHMI[READ], message, 6);
        printf("%s", message);
    }
    close(pipeInputHMI[READ]);

    return 0;
}