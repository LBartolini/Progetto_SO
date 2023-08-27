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
#include "FFR.h"
#include "SVC.h"

int velocita, parking, sospeso, tuttiComponentiConnessi;
int _log, mainSocket;
int pipeInputHMI[2];
struct Componente componenti[NUM_COMPONENTI];

void inputHandler(int);
void termHandler(int);
void setupLogFiles();
void centralECU(int);
void initProcesses(int);
void removeAllProcesses();
int checkCodiciParcheggio(char*);
void removeAllProcessessExceptPA();
int initPA(int mode);

int main(int argc, char *argv[]){
    int mode;
    velocita = 0;
    parking = 0;
    sospeso = 1;
    tuttiComponentiConnessi = 0;

    signal(SIGUSR1, inputHandler);
    signal(SIGTERM, termHandler);

    if (argc < 2) exit(EXIT_FAILURE);

    if(strcmp(argv[1], "NORMALE")==0){
        mode=NORMALE;
    }else if(strcmp(argv[1], "ARTIFICIALE")==0){
        mode=ARTIFICIALE;
    }
    mainSocket = initServerSocket(CENTRAL_SOCKET);
    initProcesses(mode);

    _log = open(ECU_LOG, O_WRONLY);
    if(_log == -1) exit(EXIT_FAILURE);
    
    centralECU(mode);
    
    termHandler(0);
}

void inputHandler(int sig){
    char message[12];
    memset(message, 0, sizeof message);
    readLine(pipeInputHMI[READ], message);

    if(tuttiComponentiConnessi && !parking){
        if(strcmp(message, "INIZIO")==0){
            sospeso=0;
        }else if(strcmp(message, "PARCHEGGIO")==0){
            parking=1;
        }else if(strcmp(message, "ARRESTO")==0){
            velocita=0;
            kill(componenti[N_BBW].pid, SIGUSR1); // segnale di arresto
            writeLine(_log, "BBW:SEGNALE ARRESTO (da HMI)");
        }else exit(EXIT_FAILURE);
    }
}

void termHandler(int sig){
    close(_log);
    close(mainSocket);
    close(pipeInputHMI[READ]);
    removeAllProcesses();
    exit(EXIT_SUCCESS);
}

void removeAllProcesses(){
    for(int i=0; i<NUM_COMPONENTI; i++){
        kill(componenti[i].pid, SIGTERM);
    }
}

void setupComponent(int pos, int pid, char* nome){
    strcpy(componenti[pos].nome, nome);
    componenti[pos].pid = pid;
    memset(componenti[pos].buffer, 0, sizeof componenti[pos].buffer);
}

int initPA(int mode){
    int pid;
    // inizializzazione parkAssist
    pid = fork();
    if (pid == 0){ 
        mainParkAssist(mode);
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);
    setupComponent(N_PA, pid, PA);
    removeAllProcessessExceptPA();
    return pid;
}

void removeAllProcessessExceptPA(){
    kill(componenti[N_FWC].pid, SIGTERM);
    kill(componenti[N_FFR].pid, SIGTERM);
    kill(componenti[N_SBW].pid, SIGTERM);
    kill(componenti[N_BBW].pid, SIGTERM);
    kill(componenti[N_TC].pid, SIGTERM);
}

int createLog(char *path){
    int fd = open(path, O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) exit(EXIT_FAILURE);
    close(fd);
    return 1;
}

void setupLogFiles(){
    remove(ECU_LOG);
    remove(SBW_LOG);
    remove(TC_LOG);
    remove(BBW_LOG);
    remove(FWC_LOG);
    remove(FFR_LOG);
    remove(PA_LOG);
    remove(SVC_LOG);

    if(!(createLog(ECU_LOG) &&
            createLog(SBW_LOG) &&
            createLog(TC_LOG) &&
            createLog(BBW_LOG) &&
            createLog(FWC_LOG) &&
            createLog(FFR_LOG) &&
            createLog(SVC_LOG) &&
            createLog(PA_LOG))){
        exit(EXIT_SUCCESS);
    }
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
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);
    close(pipeInputHMI[WRITE]);
    setupComponent(N_IHMI, pid, HMI);

    // inizializzazione BrakeByWire
    pid = fork();
    if (pid == 0){ 
        mainBrakeByWire();
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);
    setupComponent(N_BBW, pid, BBW); 

    // inizializzazione FrontWindshieldCamera
    pid = fork();
    if (pid == 0){ 
        mainFrontWindshieldCamera();
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);
    setupComponent(N_FWC, pid, FWC);

    // inizializzazione SteerByWire
    pid = fork();
    if (pid == 0){ 
        mainSteerByWire();
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);
    setupComponent(N_SBW, pid, SBW);

    // inizializzazione ThrottleControl
    pid = fork();
    if (pid == 0){ 
        mainThrottleControl();
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);
    setupComponent(N_TC, pid, TC);

    // inizializzazione forwardFacingRadar
    pid = fork();
    if (pid == 0){ 
        mainForwardFacingRadar(mode);
        exit(EXIT_SUCCESS);
    }else if(pid < 0) exit(EXIT_FAILURE);
    setupComponent(N_FFR, pid, FFR);

}

int checkCodiciParcheggio(char *buffer){
    return strstr(buffer, "172A") != NULL || strstr(buffer, "D693") != NULL || strstr(buffer, "0000") != NULL || strstr(buffer, "BDD8") != NULL || strstr(buffer, "FAEE") != NULL || strstr(buffer, "4300") != NULL;
}

void throttleBrokenHandler(int sig){
    velocita = 0;
    writeLine(_log, "TC:SEGNALE TOTALE DI ARRESTO");
    termHandler(EXIT_SUCCESS);
}

void centralECU(int mode){
    int velocitaRichiesta=0;

    // controllo signal in caso di mancato funzionamento dell'accelleratore
    signal(SIGUSR2, throttleBrokenHandler);
    writeLine(_log, "Inizio connessione ai Componenti");
    for(int i=0; i<NUM_COMPONENTI-3; i++){ // NUM_COMPONENTI-3 perchè il componente InputHMI non deve connettersi alla socket e neanche SVC e PA
        struct CompConnection tempCompConnection;

        tempCompConnection = connectToComponent(mainSocket);

        int pos;
        if(strcmp(tempCompConnection.nome, BBW)==0) {
            pos=N_BBW;
        }else if(strcmp(tempCompConnection.nome, FWC)==0) {
            pos=N_FWC;
        }else if(strcmp(tempCompConnection.nome, SBW)==0) {
            pos=N_SBW;
        }else if(strcmp(tempCompConnection.nome, TC)==0) {
            pos=N_TC;
        }else if(strcmp(tempCompConnection.nome, FFR)==0){
            pos=N_FFR;
        }else exit(EXIT_FAILURE);

        componenti[pos].fdSocket = tempCompConnection.fd;

        char toLog[64] = "Componente connesso: ";
        strcat(toLog, tempCompConnection.nome);
        writeLine(_log, toLog);
    }
    writeLine(_log, "Tutti i componenti sono connessi");
    tuttiComponentiConnessi = 1;

    while(1){
        if(parking){ //procedura di parcheggio
            writeLine(_log, "Inizio procedura di parcheggio");
            while(velocita != 0){
                writeLine(componenti[N_BBW].fdSocket, "FRENO 5");
                writeLine(_log, "BBW:FRENO 5");
                velocita -= 5;
                sleep(1);
            }
            int pidPA = initPA(mode);
            struct CompConnection tempCompConnection;
            tempCompConnection = connectToComponent(mainSocket);
            componenti[N_PA].fdSocket = tempCompConnection.fd;
            writeLine(_log, "PA:PARK");
            
            int parcheggioCompletato = 1;
            do{
                memset(componenti[N_PA].buffer, 0, sizeof componenti[N_PA].buffer);
                readLine(componenti[N_PA].fdSocket, componenti[N_PA].buffer);
                // controlla se il buffer contiene la stringa, 
                // in tal caso la procedura di parcheggio non ha funzionato 
                // e deve essere tentata nuovamente
                if(checkCodiciParcheggio(componenti[N_PA].buffer)) {
                    writeLine(_log, "Parcheggio errato!");
                    kill(componenti[N_PA].pid, SIGTERM);
                    parcheggioCompletato = 0;
                    break;
                }
                usleep(200000);
            }while(strcmp(componenti[N_PA].buffer, "END PARK")!=0);

            if(parcheggioCompletato){
                writeLine(_log, "Parcheggio Completato con successo!");
                termHandler(0);
            }
            
        }else if(!sospeso){
            if (velocita > velocitaRichiesta){
                writeLine(componenti[N_BBW].fdSocket, "FRENO 5");
                writeLine(_log, "BBW:FRENO 5");
                velocita -= 5;
            }else if (velocita < velocitaRichiesta){
                writeLine(componenti[N_TC].fdSocket, "INCREMENTO 5");
                writeLine(_log, "TC:INCREMENTO 5");
                velocita += 5;
            }

            memset(componenti[N_FWC].buffer, 0, sizeof componenti[N_FWC].buffer);
            readLine(componenti[N_FWC].fdSocket, componenti[N_FWC].buffer);

            if(isNumber(componenti[N_FWC].buffer)){
                int newVelocitaRichiesta = toNumber(componenti[N_FWC].buffer);
                if(velocitaRichiesta != newVelocitaRichiesta){
                    char toLog[100];
                    sprintf(toLog, "Velocità richiesta %d", newVelocitaRichiesta);
                    writeLine(_log, toLog);
                }
                velocitaRichiesta = newVelocitaRichiesta;
            }else{
                if(strcmp(componenti[N_FWC].buffer, "PARCHEGGIO")==0){
                    parking=1;
                }else if(strcmp(componenti[N_FWC].buffer, "PERICOLO")==0){
                    kill(componenti[N_BBW].pid, SIGUSR1);
                    writeLine(_log, "Pericolo");
                    writeLine(_log, "BBW:SEGNALE ARRESTO");
                    sospeso=1;
                    velocita=0;
                }else if(strcmp(componenti[N_FWC].buffer, "SINISTRA")==0){
                    writeLine(componenti[N_SBW].fdSocket, "SINISTRA");
                    writeLine(_log, "SBW:SINISTRA");
                }else if(strcmp(componenti[N_FWC].buffer, "DESTRA")==0){
                    writeLine(componenti[N_SBW].fdSocket, "DESTRA");
                    writeLine(_log, "SBW:DESTRA");
                }
            }

        }
        usleep(200000);
    }
}