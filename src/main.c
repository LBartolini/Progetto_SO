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
#include "utils.h"
#include "IHMI.h"
#include "BBW.h"
#include "PA.h"
#include "SBW.h"
#include "TC.h"
#include "FWC.h"

int velocita;
int pipeInputHMI[2];
int mainSocket;
struct Componente componenti[NUM_COMPONENTI];

void inputHandler(int sig);
void setupLogFiles();
void centralECU();
void initProcesses();

int main(int argc, char *argv[])
{
    int mode;
    signal(SIGUSR1, inputHandler);

    if(strcmp(argv[1], "NORMALE")==0){
        mode=NORMALE;
    }else if(strcmp(argv[1], "ARTIFICIALE")==0){
        mode=ARTIFICIALE;
    }
    mainSocket = initServerSocket("main");
    initProcesses(mode);

    // TODO: connessioni fra processi e socket centrale
    
    centralECU();
    
    close(mainSocket);
    close(pipeInputHMI[READ]);
    return 0;
}

void inputHandler(int sig){
    char message[12];
    memset(message, 0, sizeof message);
    readLine(pipeInputHMI[READ], message);

    // TODO
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
    remove(ASSIST_LOG);

    if(!(createLog(ECU_LOG) &&
            createLog(STEER_LOG) &&
            createLog(THROTTLE_LOG) &&
            createLog(BRAKE_LOG) &&
            createLog(CAMERA_LOG) &&
            createLog(ASSIST_LOG))){
        exit(0);
    }
}

void setupComponent(int pos, int pid, char* nome){
    strcpy(componenti[pos].nome, nome);
    componenti[pos].pid = pid;
    memset(componenti[pos].buffer, 0, sizeof componenti[pos].buffer);
}

void initProcesses(int mode){
    int pid;
    setupLogFiles();

    // inizializzazione HMI Input
    pipe(pipeInputHMI);
    pid = fork();
    if (pid == 0){ 
        close(pipeInputHMI[READ]);
        mainInputHMI(pipeInputHMI);
        exit(0);
    }else if(pid < 0) exit(0);
    close(pipeInputHMI[WRITE]);
    setupComponent(IHMI, pid, "INP"); // input (hmi)

    // inizializzazione BrakeByWire
    pid = fork();
    if (pid == 0){ 
        mainBrakeByWire();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(BBW, pid, "BBW"); // brake by wire

    // inizializzazione FrontWindshieldCamera
    pid = fork();
    if (pid == 0){ 
        mainFrontWindshieldCamera();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(FWC, pid, "FWC"); // front windshield camera

    // inizializzazione ParkAssist
    pid = fork();
    if (pid == 0){ 
        mainParkAssist(mode);
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(PA, pid, "PAC"); // park assist component

    // inizializzazione SteerByWire
    pid = fork();
    if (pid == 0){ 
        mainSteerByWire();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(SBW, pid, "SBW"); // steer by wire

    // inizializzazione ThrottleControl
    pid = fork();
    if (pid == 0){ 
        mainThrottleControl();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(TC, pid, "TCC"); // throttle control component

}

void centralECU(){
    int log = open(ECU_LOG, O_WRONLY);
    if(log == -1) exit(0);

    while(1){}
}

