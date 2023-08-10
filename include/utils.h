#ifndef UTIL_H
#define UTIL_H

struct CompConnection{
    char nome[4];
    int fd;
};

int writeLine(int, char*);
int readLine(int, char*);
void readLineFromIndex(int, char*, int*);
int initServerSocket(char*);
int connectToServer(char*);
struct CompConnection connectToComponent(int);

#endif