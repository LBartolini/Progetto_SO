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
#include "utils.h"

int velocita;
int mode;
int pipeInputHMI[2];

int logECU;

void inputHandler(int sig);
void setupLogFiles();
void centralECU();

int main(int argc, char *argv[])
{
    signal(SIGUSR1, inputHandler);
    setupLogFiles();

    logECU = open(ECU_LOG, O_WRONLY);
    if(logECU == -1) exit(0);

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
        exit(0);
    }
    close(pipeInputHMI[WRITE]);
    
    centralECU();
    
    close(pipeInputHMI[READ]);
    return 0;
}

void inputHandler(int sig){
    writeLine(logECU, "sigusr1 ricevuto");
    char message[12];
    memset(message, 0, sizeof message);
    readLine(pipeInputHMI[READ], message);
    writeLine(logECU, message);
}

int createLog(char *path)
{
    int fd = open(path, O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1){
        exit(0);
    }
    close(fd);
    return 1;
}

void setupLogFiles(){
    remove(ECU_LOG);
    remove(STEER_LOG);
    remove(THROTTLE_LOG);
    remove(BRAKE_LOG);
    remove(CAMERA_LOG);
    remove(RADAR_LOG);
    remove(ASSIST_LOG);
    remove(CAMERAS_LOG);

    if(!(createLog(ECU_LOG) &&
            createLog(STEER_LOG) &&
            createLog(THROTTLE_LOG) &&
            createLog(BRAKE_LOG) &&
            createLog(CAMERA_LOG) &&
            createLog(RADAR_LOG) &&
            createLog(ASSIST_LOG) &&
            createLog(CAMERAS_LOG))){
        exit(0);
    }
}

void centralECU(){
    while(1){}
}

