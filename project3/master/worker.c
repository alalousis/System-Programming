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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "structs.h"
#include "functions.h"

int main(int argc, char *argv[]){
    int i, w, bufferSize, countriesPerWorker, recordsHashtableSize, diseaseHashtableSize, countryHashtableSize, bucketSize, entriesPerBucket;
    int fdRead, fdWrite, messageSize, serverPort, workerPortNum;
    char ch, input_dir[64], argument[8][32], fifoName[16], **workerCountries, *message, serverIP[32], numWorkers[4];
    struct listNode **recordsHashtable, *currentListNode;
    struct bucket **diseaseHashtable, **countryHashtable;
    struct date date1, date2;
    struct dateFileNode *files;

    int j, k, z, querySize, sock, newsock;
    char query[100];
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    bufferSize = atoi(argv[1]);
    strcpy(input_dir, argv[2]);
    w = atoi(argv[3]);
    strcpy(numWorkers, argv[4]);

    message = malloc(bufferSize*sizeof(char));
    bucketSize = 45;
    entriesPerBucket = (bucketSize - 8) / 16;
    recordsHashtableSize = 10;
    diseaseHashtableSize = 7;
    countryHashtableSize = 10;

    strcpy(fifoName, "fifo");
    ch = '0' + w;
    strncat(fifoName, &ch, 1);
    strncat(fifoName, "Write", 5);
    if( (fdRead = open(fifoName, O_RDONLY)) < 0 ){
        perror("can't open read fifo");
    }

    strcpy(fifoName, "fifo");
    ch = '0' + w;
    strncat(fifoName, &ch, 1);
    strncat(fifoName, "Read", 4);
    if( (fdWrite = open(fifoName, O_WRONLY)) < 0 ){
        perror("can't open read fifo");
    }

    receiveServerIPnPORT(message, bufferSize, serverIP, &serverPort, fdRead);

    sleep(2);

    if( (messageSize=read(fdRead, message, bufferSize)) <= 0 ){         //Assign countries to workers
        perror("error in reading message");
    }
    message[messageSize] = '\0';
    countriesPerWorker = message[0] - '0';
    workerCountries = malloc(countriesPerWorker*sizeof(char *));
    decodeCountries(fdRead, message, bufferSize, workerCountries, countriesPerWorker);

    recordsHashtable = malloc(recordsHashtableSize*sizeof(struct listNode*));
    for(i=0; i<recordsHashtableSize; i++){
        recordsHashtable[i] = NULL;
    }

    diseaseHashtable = malloc(diseaseHashtableSize*sizeof(struct bucket*));
    for(i=0; i<diseaseHashtableSize; i++){
        diseaseHashtable[i] = NULL;
    }

    countryHashtable = malloc(countryHashtableSize*sizeof(struct bucket*));
    for(i=0; i<countryHashtableSize; i++){
        countryHashtable[i] = NULL;
    }

    saveRecords(files, recordsHashtable, recordsHashtableSize, input_dir, workerCountries, countriesPerWorker);

    for(i=0; i<recordsHashtableSize; i++){
        currentListNode = recordsHashtable[i];
        while(currentListNode != NULL){
            Hash(diseaseHashtable, diseaseHashtableSize, currentListNode->record->disease, entriesPerBucket, currentListNode->record);
            Hash(countryHashtable, countryHashtableSize, currentListNode->record->country, entriesPerBucket, currentListNode->record);
            currentListNode = currentListNode->nextRecord;
        }
    }
    strcpy(message, "");
    workerPortNum = 5000 + w;

    sendStatistics(workerPortNum, serverPort, serverIP, numWorkers);

    strcpy(message, "");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error in socket");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(workerPortNum);
    if(bind(sock, serverptr, sizeof(server)) < 0){
        perror("Error in bind");
    }

    if(listen(sock, 5) < 0){
        perror("Error in listen");
    }

    while(1){
        clientlen = sizeof(client);
        if((newsock = accept(sock, clientptr, &clientlen)) < 0){
            perror("Error in accept");
        }

        query[0] = '\0';
        if((querySize=read(newsock, query, 100)) < 0){
            perror("Error in read");
        }
        query[querySize] = '\0';

        for(k=0; k<=7; k++){
            strcpy(argument[k], "");
        }

        j=0;
        z=0;
        for(i=0; i<querySize; i++){
            if(query[i] == ' '){
                argument[j][z] = '\0';
                j++;
                i++;
                z=0;
            }
            argument[j][z] = query[i];
            z++;
        }
        argument[j][z] = '\0';
        strcpy(message, "");
        if(strcmp(argument[0], "/diseaseFrequency") == 0){
            if(checkDate(argument[2], argument[3], message, fdWrite) ){
                diseaseFrequency(diseaseHashtable, diseaseHashtableSize, entriesPerBucket, argument[1], argument[2],  argument[3],  argument[4], message, newsock);
            }

        }
        else if(strcmp(argument[0], "/topk-AgeRanges") == 0){
            if(countryOfWorker(workerCountries, countriesPerWorker, argument[2]) == 0){
                if(write(newsock, "n", 1) != 1){
                    perror("error in writing message");
                }
                continue;
            }

            if(checkDate(argument[4], argument[5], message, newsock) ){
                topAgeRanges(diseaseHashtable, diseaseHashtableSize, entriesPerBucket, atoi(argument[1]), argument[3], argument[2], argument[4], argument[5], message, newsock);
            }
        }

        else if(strcmp(argument[0], "/searchPatientRecord") == 0){
            searchByID(recordsHashtable, recordsHashtableSize, argument[1], message, newsock);
        }
        else if(strcmp(argument[0], "/numPatientAdmissions") == 0){
            if(checkDate(argument[2], argument[3], message, newsock) ){
                numPatientAdmissions(countryHashtable, countryHashtableSize, entriesPerBucket, argument[1], argument[2], argument[3], argument[4], message, newsock);
            }

        }
        else if(strcmp(argument[0], "/numPatientDischarges") == 0){
            if(checkDate(argument[2], argument[3], message, newsock) ){
                numPatientDischarges(countryHashtable, countryHashtableSize, entriesPerBucket, argument[1], argument[2], argument[3], argument[4], message, newsock);
            }
        }
        else if(strcmp(argument[0], "/exit") != 0){
                //Wrong Command !
        }
        else if(strcmp(argument[0], "/exit") == 0){
            break;
        }
    }

    freeRecordsHashTable(recordsHashtable, recordsHashtableSize);
    freeHashTable(diseaseHashtable, diseaseHashtableSize, entriesPerBucket);
    freeHashTable(countryHashtable, countryHashtableSize, entriesPerBucket);
    exit(1);
}
