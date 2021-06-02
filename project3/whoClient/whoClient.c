#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "functions.h"
#include "structs.h"
#include "variables.h"

extern int queriesRead;

int main(int argc, char *argv[]){
    int i, numThreads, servPort, sock, bufferSize, numQueries, error, queriesPerThread;
    char queryFile[16], *buffer, servIP[20], **queries;
    pthread_t *tids;
    struct arguments args;

    for(i=1; i<argc-1; i++){
        if(strcmp(argv[i], "-q") == 0){
                strcpy(queryFile, argv[i+1]);
        }
        else if(strcmp(argv[i], "-w") == 0){
                numThreads = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-sp") == 0){
                servPort = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-sip") == 0){
                strcpy(servIP, argv[i+1]);
        }
    }

    bufferSize = 1024;
    buffer = malloc(bufferSize*sizeof(char));

    numQueries = countQueries(queryFile);

    queries = malloc(numQueries*sizeof(char *));
    for(i=0; i<numQueries; i++){
        queries[i] = malloc(100*sizeof(char));
    }

    readQueries(queryFile, queries, numQueries);

    queriesRead = 0;
    pthread_mutex_init(&read_mutex, NULL);
    pthread_cond_init(&read_cond, NULL);
    tids = malloc(numThreads*sizeof(pthread_t));
    args.numQueries = numQueries;
    args.queries = queries;
    args.numThreads = numThreads;
    args.servPort = servPort;
    args.servIP = servIP;

    for(i=0; i<numThreads; i++){
        if(error = pthread_create(&tids[i], NULL, threadRoutine, (void *)&args)){
            printf("Error\n");
            terror("pthread_create", error);
        }
    }

    for(i=0; i<numThreads; i++){
            if(error = pthread_join(tids[i], NULL)){
            terror("pthread_join", error);
        }
    }
}
