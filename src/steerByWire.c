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
#include "SBW.h"
#include "utils.h"

void mainSteerByWire(){
    int sock, logSBW;

    logSBW = open(SBW_LOG, O_WRONLY);
    if(logSBW == -1) exit(0);

    writeLine(logSBW, "Connessione alla ECU");
    sock = connectToServer(CENTRAL_SOCKET);
    writeLine(sock, SBW);
    writeLine(logSBW, "Connessione stabilita con successo");
}