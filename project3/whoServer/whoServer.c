#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "structs.h"
#include "functions.h"
#include "variables.h"

extern int numWorkers;
extern int *workerPorts;

int main(int argc, char *argv[]){
    int i, j, z, counter, queryPortNum, statisticsPortNum, numThreads, bufferSize, sock, sock1, newsock, newsock1, querySize, error, workerPortNum, messageSize;
    char ch, query[100], workerPort[8], workerIP[32], message[100], numWorkersStr[4];
    pthread_t *tids, maintThread;
    struct arguments args;
    struct sockaddr_in server1, server, client1, client;
    socklen_t clientlen1, clientlen, serverlen1, serverlen, clientAddresslen;
    struct sockaddr *serverptr1 = (struct sockaddr *)&server1;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr1 = (struct sockaddr *)&client1;
    struct sockaddr *clientptr = (struct sockaddr *)&client;
    struct hostent *rem;

    for(i=1; i<argc-1; i++){
        if(strcmp(argv[i], "-q") == 0){
                queryPortNum = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-s") == 0){
                statisticsPortNum = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-w") == 0){
                numThreads = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-b") == 0){
                bufferSize = atoi(argv[i+1]);
        }
    }

    tids = malloc(numThreads*sizeof(pthread_t));
    init(&cirBuff, bufferSize);
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond_nonfull, NULL);
    pthread_cond_init(&cond_nonempty, NULL);


    if ((sock1 = socket(AF_INET, SOCK_STREAM, 0)) < 0){              //Receive worker port num
        perror("Error in socket");
    }

    server1.sin_family = AF_INET;
    server1.sin_addr.s_addr = htonl(INADDR_ANY);
    server1.sin_port = htons(statisticsPortNum);
    serverlen1 = sizeof(server1);
    if(bind(sock1, serverptr1, serverlen1) < 0){
        perror("Error in bind 1");
    }

    if(listen(sock1, 5) < 0){
        perror("Error in listen");
    }
    counter = 0;
    printf("Ready to communicate with Workers\n");
    while(1){
        clientlen1 = sizeof(client1);
        if((newsock1 = accept(sock1, clientptr1, &clientlen1)) < 0){
            perror("Error in accept");
        }

        place(&cirBuff, newsock1, bufferSize);
        pthread_cond_broadcast(&cond_nonempty);
        sleep(2);
        counter++;

        message[0] = '\0';
        while(read(newsock1, &ch, 1) > 0){
            strncat(message, &ch, 1);
            if(ch == '$'){
                break;
            }
        }
        message[strlen(message)] = '\0';

        clientAddresslen = 32;
        if(getpeername(newsock1, clientptr1, &clientAddresslen) < 0){     //get workerIP
            perror("Error in getpeername");
        }
        strcpy(workerIP, inet_ntoa(client1.sin_addr));

        j=0;
        z=0;
        while(1){
            while(message[z] != '#'){
                workerPort[j] = message[z];
                j++;
                z++;
            }
            workerPort[j] = '\0';

            z++;
            j=0;
            while(message[z] != '$'){
                numWorkersStr[j] = message[z];
                j++;
                z++;
            }
            numWorkersStr[j] = '\0';
            break;
        }
        numWorkers = atoi(numWorkersStr);
        workerPortNum = atoi(workerPort);

        if(counter == numWorkers){
            break;
        }
    }

    workerPorts = malloc(numWorkers*sizeof(int));
    args.bufferSize = bufferSize;
    strcpy(args.workerIP, workerIP);
    for(i=0; i<numThreads; i++){
        if(error = pthread_create(&tids[i], NULL, threadRoutine, (void *)&args)){
            printf("Error\n");
            terror("pthread_create", error);
        }
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error in socket");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(queryPortNum);
    if(bind(sock, serverptr, sizeof(server)) < 0){
        perror("Error in bind");
    }

    if(listen(sock, 5) < 0){
        perror("Error in listen");
    }

    printf("Ready to communicate with whoClient\n");

    while(1){
        clientlen = sizeof(client);
        if((newsock = accept(sock, clientptr, &clientlen)) < 0){
            perror("Error in accept");
        }

        place(&cirBuff, newsock, bufferSize);
        pthread_cond_broadcast(&cond_nonempty);
        sleep(2);
    }

    for(i=0; i<numThreads; i++){
        if(error = pthread_join(tids[i], NULL)){
            terror("pthread_join", error);
        }
    }

    if(error = pthread_mutex_destroy(&mtx)){
        terror("pthread_mutex_destroy", error);
    }

    pthread_exit(NULL);
}
