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
#include <time.h>
#include <limits.h>

#include "definitions.h"
#include "TC.h"
#include "utils.h"

int sock, logTC;
char buffer[100], toLog[60];

void termHandlerTC(int);
int throttleCrash();

void mainThrottleControl(){
    char toAppend[1];
    signal(SIGTERM, termHandlerTC);

    logTC = open(TC_LOG, O_WRONLY);
    if(logTC == -1) exit(EXIT_FAILURE);

    writeLine(logTC, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, TC);
    writeLine(logTC, "Connessione stabilita con successo");

    while(1){
        strcpy(toLog, "AUMENTO ");
        memset(buffer, 0, sizeof buffer);
        memset(toAppend, 0, sizeof toAppend);
        readLine(sock, buffer);
        if(throttleCrash()){
            kill(getppid(), SIGUSR1);
            break;
        }
        int i = 11;
        while(buffer[i] != '\0'){
            sprintf(toAppend, "%c", buffer[i]);
            strcat(toLog, toAppend);
            i++;
        }
        sprintf(buffer, "%d:%s", (int)time(NULL), toLog);
        writeLine(logTC, buffer);
    }

    termHandlerTC(0);
}

int throttleCrash(){
    unsigned int randomNumber = 0;
    for (int i = 0; i < 4; ++i) {
        // Genera un numero randomico di 2 byte e shifta a sx per far spazio al successivo
        unsigned int randomByte = rand() & 0xFF;
        randomNumber = (randomNumber << 8) | randomByte;
    }
    // rangeAccepted ha un valore di 42949.67295 esattamente 1/10^5 rispetto a randomNumber che può valere fino a 4,294,967,295
    double rangeAccepted =  (double)UINT_MAX / pow(10,5);
    return randomNumber < rangeAccepted;
}

void termHandlerTC(int sig){
    close(sock);
    close(logTC);
    exit(EXIT_SUCCESS);
}