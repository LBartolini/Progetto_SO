#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "utils.h"
#include "definitions.h"

int writeLine(int fd, char *string){
    size_t len = strlen(string);
    char *newstr = malloc(len + 2);
    strcpy(newstr, string);
    strcat(newstr, "\n");

    return write(fd, newstr, len + 1);
}

int readLine(int fd, char *string){
    int n;
    while (1)
    {
        n = read(fd, string, 1);
        if (n <= 0 || *string == 10 || *string == '\n' || *string == '\0'){
            *string = '\0';
            break;
        }
        string++;
    }

    return n > 0;
}

void readLineFromIndex(int fd, char *str, int *index) {
    int n;
    lseek(fd, *index, SEEK_SET);
    do{
        n = read(fd, str, 1);
        if (n < 0){
            exit(0);
        }
    } while (n > 0 && *str++ != '\0');
}

int initServerSocket(char *name){
    int centralFd;
    socklen_t centralLen;
    struct sockaddr_un centralAddr;
    centralLen = sizeof(centralAddr);
    centralFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (centralFd < 0) exit(0);
    centralAddr.sun_family = AF_UNIX;
    strcpy(centralAddr.sun_path, name);
    unlink(centralAddr.sun_path);
    if (bind(centralFd, (struct sockaddr *)&centralAddr, centralLen) < 0) exit(0);
    if (listen(centralFd, NUM_COMPONENTI) < 0) exit(0);

    return centralFd;
}

int connectToServer(char *name){
    int clientFd, serverLen, result;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr *serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr *)&serverUNIXAddress;
    serverLen = sizeof(serverUNIXAddress);
    clientFd = socket(AF_UNIX, SOCK_STREAM, 0);
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy(serverUNIXAddress.sun_path, name);
    do{
        result = connect(clientFd, serverSockAddrPtr, serverLen);
        if (result == -1) 
            sleep(1);
    } while (result == -1);

    return clientFd;
}

struct CompConnection connectToComponent(int centralFd){
    struct CompConnection* c;
    int clientFd;
    socklen_t clientLen;
    struct sockaddr_un clientAddr;
    clientLen = sizeof(clientAddr);
    clientFd = accept(centralFd, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientFd < 0) exit(0);

    char buffer[1024];
    memset(buffer, 0, sizeof buffer);
    while (readLine(clientFd, buffer) <= 0) sleep(1);

    c = malloc(sizeof (struct CompConnection));
    strcpy((*c).nome, buffer);
    (*c).fd = clientFd;

    return *c;
}

int sendMessage(int fd, char* nome, char* msg){
    char* result;
    result = malloc((sizeof nome)+(sizeof msg)+1);
    strcpy(result, nome);
    strcat(result, ":");
    strcat(result, msg);

    return writeLine(fd, result);
}