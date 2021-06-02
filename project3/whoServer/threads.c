#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "structs.h"
#include "functions.h"
#include "variables.h"

extern int flag;
extern int numWorkers;
extern int *workerPorts;

void terror(char *s, int error){
    fprintf(stderr, "%s: %s\n", s, strerror(error));
}

void *threadRoutine(void *arguments){
    int i, j, k, newsock, bufferSize, sock, workerPortNum, *sockets, error;
    char query[100], ch, workerIP[32], argument0[32], message[100];
    struct arguments *args;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    args = (struct arguments *)arguments;
    bufferSize = args->bufferSize;
    strcpy(workerIP, args->workerIP);
    flag = 0;
    sockets = malloc(numWorkers*sizeof(int));

    while(flag == 0){
        newsock = obtain(&cirBuff, bufferSize);
        if(newsock>=4 && newsock <=4+numWorkers){
            workerPorts[newsock-4] = receiveStatistics(newsock);
            continue;
        }

        query[0] = '\0';
        while(read(newsock, &ch, 1) > 0){
            strncat(query, &ch, 1);
        }
        query[strlen(query)] = '\0';

        if(strcmp(query, "/exit") == 0){
            flag = 1;
        }
        pthread_cond_broadcast(&cond_nonfull);
        sleep(2);

        if(error = pthread_mutex_lock(&mtx2)){
            terror("pthread_mutex_unlock", error);
        }
        sendQuery(workerIP, workerPorts, query, numWorkers, sockets);      //send query to worker

        sleep(2);
        strcpy(argument0, "");
        strcpy(message, "");

        k=0;
        j=0;
        while(query[j] != 32 && query[j] != 0){
            argument0[k] = query[j];
            k++;
            j++;
        }
        argument0[k] = '\0';


        strcpy(message, "");
        printf("%s\n", query);

        if(strcmp(argument0, "/diseaseFrequency") == 0){
            diseaseFrequencyReceive(sockets, numWorkers);
        }
        else if(strcmp(argument0, "/topk-AgeRanges") == 0){
            topAgeRangesReceive(sockets, numWorkers);
        }
        else if(strcmp(argument0, "/searchPatientRecord") == 0){
            searchByIDReceive(sockets, numWorkers);
        }
        else if(strcmp(argument0, "/numPatientAdmissions") == 0){
            numPatientAdmissionsReceive(sockets, numWorkers);
        }
        else if(strcmp(argument0, "/numPatientDischarges") == 0){
            numPatientDischargesReceive(sockets, numWorkers);
        }
        else if(strcmp(argument0, "/exit") != 0){
            printf("Wrong Command !!\n");
        }
        sleep(3);
        if(error = pthread_mutex_unlock(&mtx2)){
            terror("pthread_mutex_unlock", error);
        }
    }
}
