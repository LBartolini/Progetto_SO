#ifndef definitions_H
#define definitions_H

#define READ 0
#define WRITE 1

// componenti
#define NUM_COMPONENTI 6
#define IHMI 0
#define BBW 1
#define FWC 2
#define PA 3
#define SBW 4
#define TC 5

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
#define STEER_LOG "log/steer.log" 
#define THROTTLE_LOG "log/throttle.log" 
#define BRAKE_LOG "log/brake.log" 
#define CAMERA_LOG "log/camera.log" 
#define CAR_SPEED "log/car_speed"
#define ASSIST_LOG "log/assist.log" 

// componente
struct Componente{
    char nome[4];
    int pid;
    int fdSocket;
    char buffer[1024];
};

#endif