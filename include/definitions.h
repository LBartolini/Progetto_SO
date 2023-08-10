#ifndef definitions_H
#define definitions_H

#define READ 0
#define WRITE 1

#define CENTRAL_SOCKET "main"

// componenti
#define NUM_COMPONENTI 6
#define N_IHMI 0
#define N_BBW 1
#define N_FWC 2
#define N_PA 3
#define N_SBW 4
#define N_TC 5

// modalità di avvio
#define NORMALE 0
#define INPUT_NORMALE "/dev/random"
#define INPUT_NORMALE_U "/dev/urandom"

#define ARTIFICIALE 1
#define INPUT_ARTIFICIALE "data/randomARTIFICIALE.binary"
#define INPUT_ARTIFICIALE_U "data/urandomARTIFICIALE.binary"

#define FRONT_CAMERA_DATA "res/frontCamera.data"

// log
#define ECU_LOG "log/ECU.log" 
#define SBW_LOG "log/SBW.log" 
#define TC_LOG "log/TC.log" 
#define BBW_LOG "log/BBW.log" 
#define FWC_LOG "log/FWC.log" 
#define PA_LOG "log/PA.log" 

// componente
struct Componente{
    char nome[4];
    int pid;
    int fdSocket;
    char buffer[1024];
};

#endif