#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "structs.h"
#include "functions.h"

int main(int argc, char *argv[]){           //I am Parent

    int i, j, w, k, l, z, numWorkers, bufferSize, numCountries, countriesPerWorker;
    int status, messageSize, maxReadingFd, maxWritingFd;
    int **fd;      //fifo descriptors
    char ch, serverIP[32], serverPort[8], input_dir[64], query[128], argument[8][32], fifoName[16], *message, argument0[32];
    pid_t pid, *childPID;
    fd_set readingFds, writingFds;

    numWorkers = atoi(argv[1]);
    bufferSize = atoi(argv[2]);
    strcpy(serverIP, argv[3]);
    strcpy(serverPort, argv[4]);
    strcpy(input_dir, argv[5]);

    FD_ZERO(&readingFds);
    FD_ZERO(&writingFds);

    message = malloc(bufferSize*sizeof(char));
    fd = malloc(numWorkers*sizeof(int *));

    maxReadingFd=0;
    maxWritingFd=0;
    for(i=0; i<numWorkers; i++){
        fd[i] = malloc(2*sizeof(int));  //fd[i][0]=read fd[i][1]=write
        strcpy(fifoName, "fifo");
        ch = '0' + i;
        strncat(fifoName, &ch, 1);
        strncat(fifoName, "Write", 5);
        if( (fd[i][1] = open(fifoName, O_WRONLY) ) < 0 ){
            perror("can't open write fifo");
        }
        FD_SET(fd[i][1], &writingFds);
        if(fd[i][1] > maxWritingFd){
            maxWritingFd = fd[i][1];
        }

        strcpy(fifoName, "fifo");
        ch = '0' + i;
        strncat(fifoName, &ch, 1);
        strncat(fifoName, "Read", 4);
        if( (fd[i][0] = open(fifoName, O_RDONLY)) < 0 ){
            perror("can't open read fifo");
        }
        FD_SET(fd[i][0], &readingFds);
        if(fd[i][0] > maxReadingFd){
            maxReadingFd = fd[i][0];
        }
    }
    strcpy(message, "");
    sendServerIPnPORT(numWorkers, message, bufferSize, serverIP, serverPort, fd);
    strcpy(message, "");
    sleep(2);
    assignCountries(input_dir, numWorkers, message, bufferSize, fd);

    for(i=0; i<numWorkers; i++){
        close(fd[i][1]);
        strcpy(fifoName, "fifo");
        ch = '0' + i;
        strncat(fifoName, &ch, 1);
        strncat(fifoName, "Write", 5);
        unlink(fifoName);

        close(fd[i][0]);
        strcpy(fifoName, "fifo");
        ch = '0' + i;
        strncat(fifoName, &ch, 1);
        strncat(fifoName, "Read", 4);
        unlink(fifoName);
        free(fd[i]);
    }
    if( (pid=wait(&status)) == -1){
        perror("wait failed");
        exit(1);
    }

    free(fd);
}
