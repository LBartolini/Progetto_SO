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

int main(){
    char string[512];
    int len = 0;
    int fd = open(ECU_LOG, O_RDONLY);
    if(fd == -1) exit(EXIT_FAILURE);

    while(1){
        memset(string, 0, sizeof string);
        off_t pos = lseek(fd, (size_t)0, SEEK_END);
        readLineFromIndex(fd, string, &len);
        if (pos > len){
            len = pos;
            printf("%s", string);
        }
    }

    close(fd);
    return 0;
}