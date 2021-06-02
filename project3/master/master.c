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

int main(int argc, char *argv[]){
    int i, w, numWorkers;
    int **fd;      //fifo descriptors
    char ch, bufferSize[8], serverIP[32], serverPort[8], input_dir[64], fifoName[16], arg0[10], noWorkers[4], workedID[4], numWorkersStr[4];
    pid_t pid;

    for(i=1; i<argc-1; i++){
        if(strcmp(argv[i], "-w") == 0){
                numWorkers = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-b") == 0){
                strcpy(bufferSize, argv[i+1]);
        }
        else if(strcmp(argv[i], "-s") == 0){
                strcpy(serverIP, argv[i+1]);
        }
        else if(strcmp(argv[i], "-p") == 0){
                strcpy(serverPort, argv[i+1]);
        }
        else if(strcmp(argv[i], "-i") == 0){
                strcpy(input_dir, argv[i+1]);
        }
    }

    sprintf(noWorkers, "%d", numWorkers);

    for(i=0; i<numWorkers; i++){
        strcpy(fifoName, "fifo");
        ch = '0' + i;
        strncat(fifoName, &ch, 1);
        strncat(fifoName, "Read", 4);
        if( (mkfifo(fifoName, 0666) < 0) && (errno != EEXIST) ){
            perror("can't create fifo");
        }

        strcpy(fifoName, "fifo");
        ch = '0' + i;
        strncat(fifoName, &ch, 1);
        strncat(fifoName, "Write", 5);
        if( (mkfifo(fifoName, 0666) < 0) && (errno != EEXIST) ){
            perror("can't create fifo");
        }
    }

    for(w=0; w<numWorkers; w++){
        if((pid=fork()) <= 0){
            break;
        }
    }

    if(pid == 0){           // I am worker
        sprintf(workedID, "%d", w);
        sprintf(numWorkersStr, "%d", numWorkers);
        if(execlp("./worker", "./worker", bufferSize, input_dir, workedID, numWorkersStr, NULL) == -1){
            perror("Exec error");
        }
    }
    else{           //I am Parent
        if(execlp("./parent", "./parent", noWorkers, bufferSize, serverIP, serverPort, input_dir,  NULL) == -1){
            perror("Exec error");
        }
    }

    return 0;
}
