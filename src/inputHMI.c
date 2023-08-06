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

void mainInputHMI(int* pipe){
    for(int i=0; i<2; i++){
        write(pipe[WRITE], "pippo", 6);
    }

    close(pipe[WRITE]);
    exit(0);
}