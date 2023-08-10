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
int _log;
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
    mainSocket = initServerSocket(CENTRAL_SOCKET);
    initProcesses(mode);

    _log = open(ECU_LOG, O_WRONLY);
    if(_log == -1) exit(0);
    
    centralECU();
    
    close(mainSocket);
    close(pipeInputHMI[READ]);
    return 0;
}

void inputHandler(int sig){
    char message[12];
    memset(message, 0, sizeof message);
    readLine(pipeInputHMI[READ], message);
    writeLine(_log, message);
}

int createLog(char *path)
{
    int fd = open(path, O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) exit(0);
    close(fd);
    return 1;
}

void setupLogFiles(){
    remove(ECU_LOG);
    remove(SBW_LOG);
    remove(TC_LOG);
    remove(BBW_LOG);
    remove(FWC_LOG);
    remove(PA_LOG);

    if(!(createLog(ECU_LOG) &&
            createLog(SBW_LOG) &&
            createLog(TC_LOG) &&
            createLog(BBW_LOG) &&
            createLog(FWC_LOG) &&
            createLog(PA_LOG))){
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
    setupComponent(N_IHMI, pid, HMI);

    // inizializzazione BrakeByWire
    pid = fork();
    if (pid == 0){ 
        mainBrakeByWire();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(N_BBW, pid, BBW); 

    // inizializzazione FrontWindshieldCamera
    pid = fork();
    if (pid == 0){ 
        mainFrontWindshieldCamera();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(N_FWC, pid, FWC);

    // inizializzazione ParkAssist
    pid = fork();
    if (pid == 0){ 
        mainParkAssist(mode);
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(N_PA, pid, PA);

    // inizializzazione SteerByWire
    pid = fork();
    if (pid == 0){ 
        mainSteerByWire();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(N_SBW, pid, SBW);

    // inizializzazione ThrottleControl
    pid = fork();
    if (pid == 0){ 
        mainThrottleControl();
        exit(0);
    }else if(pid < 0) exit(0);
    setupComponent(N_TC, pid, TC);

}

void centralECU(){
    // TODO: connessioni fra processi e socket centrale
    writeLine(_log, "Inizio connessione ai Componenti");
    for(int i=0; i<NUM_COMPONENTI-1; i++){ // NUM_COMPONENTI-1 perchè il componente InputHMI non deve connettersi alla socket
        struct CompConnection tempCompConnection;
        struct Componente tempComponente;

        tempCompConnection = connectToComponent(mainSocket);

        if(strcmp(tempCompConnection.nome, BBW)==0) {
            tempComponente=componenti[N_BBW];
        }else if(strcmp(tempCompConnection.nome, FWC)==0) {
            tempComponente=componenti[N_FWC];
        }else if(strcmp(tempCompConnection.nome, PA)==0) {
            tempComponente=componenti[N_PA];
        }else if(strcmp(tempCompConnection.nome, SBW)==0) {
            tempComponente=componenti[N_SBW];
        }else if(strcmp(tempCompConnection.nome, TC)==0) {
            tempComponente=componenti[N_TC];
        }else exit(0);

        tempComponente.fdSocket = tempCompConnection.fd;

        char toLog[64] = "Componente connesso: ";
        strcat(toLog, tempCompConnection.nome);
        writeLine(_log, toLog);
    }
    writeLine(_log, "Tutti i componenti sono connessi");

    while(1){}
}

