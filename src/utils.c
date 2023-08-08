#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "utils.h"

int writeLine(int fd, char *string)
{
    size_t len = strlen(string);
    char *newstr = malloc(len + 2);
    strcpy(newstr, string);
    strcat(newstr, "\n");

    return write(fd, newstr, len + 1);
}

int readLine(int fd, char *string)
{
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