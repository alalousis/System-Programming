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

int flag = 0;
int sigusrFlag = 0;

void catchinterrupt(){
    flag++;
}

void catchSIGUSR1(){
    sigusrFlag++;
}

int main(int argc, char *argv[]){
    int i, j, w, k, l, z, numWorkers, bufferSize, numCountries, countriesPerWorker, recordsHashtableSize, diseaseHashtableSize, countryHashtableSize, bucketSize, entriesPerBucket;
    int status, messageSize, total, success, fail;
    int **fd;      //fifo descriptors
    char ch, input_dir[64], query[128], argument[8][32], fifoName[16], **countries, **workerCountries, *message, argument0[32];
    struct listNode **recordsHashtable, *currentListNode;
    struct bucket **diseaseHashtable, **countryHashtable;
    struct date date1, date2;
    static struct sigaction act, act2;
    pid_t pid, *childPID;

    for(i=1; i<argc-1; i++){
        if(strcmp(argv[i], "-w") == 0){
                numWorkers = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-b") == 0){
                bufferSize = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-i") == 0){
                strcpy(input_dir, argv[i+1]);
        }
    }

    bucketSize = 45;
    entriesPerBucket = (bucketSize - 8) / 16;
    recordsHashtableSize = 10;
    diseaseHashtableSize = 7;
    countryHashtableSize = 10;
    total = 0;
    success = 0;
    fail = 0;
    message = malloc(bufferSize*sizeof(char));
    childPID = malloc(numWorkers*sizeof(pid_t));

    fd = malloc(numWorkers*sizeof(int *));
    for(i=0; i<numWorkers; i++){
        fd[i] = malloc(2*sizeof(int));  //fd[i][0]=read fd[i][1]=write
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
        childPID[w] = pid;
    }

    if(pid == 0){           // I am worker
        int fdRead, fdWrite;
        char fifoName[16], ch, country[16];
        struct dateFileNode *files;

        act.sa_handler=catchinterrupt;
        sigfillset(&(act.sa_mask));
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGQUIT, &act, NULL);

        act2.sa_handler=catchSIGUSR1;
        sigfillset(&(act2.sa_mask));
        sigaction(SIGUSR1, &act2, NULL);

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
        sleep(2);
        statisticsSend(countryHashtable, countryHashtableSize, diseaseHashtable, diseaseHashtableSize, entriesPerBucket, message, fdWrite);
        strcpy(message, "");

        do{
            if(flag == 1){
                if(write(fdWrite, "n", 1) != 1){
                    perror("error in writing message");
                }
                break;
            }

            if(sigusrFlag == 1){
                sigusr1(files, input_dir, workerCountries, countriesPerWorker, recordsHashtable, recordsHashtableSize);

                freeHashTable(diseaseHashtable, diseaseHashtableSize, entriesPerBucket);
                freeHashTable(countryHashtable, countryHashtableSize, entriesPerBucket);
                diseaseHashtable = malloc(diseaseHashtableSize*sizeof(struct bucket*));
                for(i=0; i<diseaseHashtableSize; i++){
                    diseaseHashtable[i] = NULL;
                }

                countryHashtable = malloc(countryHashtableSize*sizeof(struct bucket*));
                for(i=0; i<countryHashtableSize; i++){
                    countryHashtable[i] = NULL;
                }

                for(i=0; i<recordsHashtableSize; i++){
                    currentListNode = recordsHashtable[i];
                    while(currentListNode != NULL){
                        Hash(diseaseHashtable, diseaseHashtableSize, currentListNode->record->disease, entriesPerBucket, currentListNode->record);
                        Hash(countryHashtable, countryHashtableSize, currentListNode->record->country, entriesPerBucket, currentListNode->record);
            			currentListNode = currentListNode->nextRecord;
            		}
                }
                sigusrFlag--;
            }

            strcpy(message, "");
            receiveCommand(fdRead, argument, message, bufferSize);
            total++;
            strcpy(message, "");

            if(strcmp(argument[0], "/listCountries") == 0){
                listCountries(workerCountries, countriesPerWorker, message, fdWrite);
                success++;
            }
            else if(strcmp(argument[0], "/diseaseFrequency") == 0){
                if(checkDate(argument[2], argument[3], message, fdWrite) ){
                    diseaseFrequency(diseaseHashtable, diseaseHashtableSize, entriesPerBucket, argument[1], argument[2],  argument[3],  argument[4], message, fdWrite);
                    success++;
                }
                else{
                    fail++;
                }

            }
            else if(strcmp(argument[0], "/topk-AgeRanges") == 0){
                if(countryOfWorker(workerCountries, countriesPerWorker, argument[2]) == 0){
                    if(write(fdWrite, "n", 1) != 1){
                        perror("error in writing message");
                    }
                    continue;
                }

                if(checkDate(argument[4], argument[5], message, fdWrite) ){
                    topAgeRanges(diseaseHashtable, diseaseHashtableSize, entriesPerBucket, atoi(argument[1]), argument[3], argument[2], argument[4], argument[5], message, fdWrite);
                    success++;
                }
                else{
                    fail++;
                }
            }

            else if(strcmp(argument[0], "/searchPatientRecord") == 0){
                searchByID(recordsHashtable, recordsHashtableSize, argument[1], message, fdWrite);
                success++;
            }
            else if(strcmp(argument[0], "/numPatientAdmissions") == 0){
                if(checkDate(argument[2], argument[3], message, fdWrite) ){
                    numPatientAdmissions(countryHashtable, countryHashtableSize, entriesPerBucket, argument[1], argument[2], argument[3], argument[4], message, fdWrite);
                    success++;
                }
                else{
                    fail++;
                }
            }
            else if(strcmp(argument[0], "/numPatientDischarges") == 0){
                if(checkDate(argument[2], argument[3], message, fdWrite) ){
                    numPatientDischarges(countryHashtable, countryHashtableSize, entriesPerBucket, argument[1], argument[2], argument[3], argument[4], message, fdWrite);
                    success++;
                }
                else{
                    fail++;
                }

            }
            else if(strcmp(argument[0], "/exit") != 0){
                fail++;     //Wrong Command !
            }
        }while(strcmp(argument[0], "/exit") != 0);

        logfileChild(workerCountries, countriesPerWorker, total, success, fail);
        freeRecordsHashTable(recordsHashtable, recordsHashtableSize);
        freeHashTable(diseaseHashtable, diseaseHashtableSize, entriesPerBucket);
        freeHashTable(countryHashtable, countryHashtableSize, entriesPerBucket);
        exit(1);
    }
    else{           //I am Parent
        act.sa_handler=catchinterrupt;
        sigfillset(&(act.sa_mask));
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGQUIT, &act, NULL);

        fd_set readingFds, writingFds;
        int maxReadingFd, maxWritingFd;
        FD_ZERO(&readingFds);
        FD_ZERO(&writingFds);

        maxReadingFd=0;
        maxWritingFd=0;
        for(i=0; i<numWorkers; i++){
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

        assignCountries(input_dir, numWorkers, message, bufferSize, fd);

        strcpy(message, "");
        statisticsReceive(fd, message, bufferSize, numWorkers);
        strcpy(message, "");

        do{
            printf("Please give command\n");
            strcpy(argument0, "");
            strcpy(query, "");
            strcpy(message, "");

            scanf(" %[^\n]s", query);
            total++;

            i=0;
            j=0;
            while(query[j] != 32 && query[j] != 0){
                argument0[i] = query[j];
                i++;
                j++;
            }
            argument0[i] = '\0';

            if(strcmp(argument0, "/exit") == 0){
                for(i=0; i<numWorkers; i++){
                    kill(childPID[i], SIGKILL);
                }
                break;
            }

            sendCommand(fd, &writingFds, maxWritingFd, query, message, bufferSize, numWorkers);

            strcpy(message, "");
            if(strcmp(argument0, "/listCountries") == 0){
                if( listCountriesReceive(fd, &readingFds, maxReadingFd, message, bufferSize, numWorkers) ){
                    success++;
                }
                else{
                    fail++;
                }
            }
            else if(strcmp(argument0, "/diseaseFrequency") == 0){
                if( diseaseFrequencyReceive(fd, &readingFds, maxReadingFd, numWorkers, message, bufferSize) ){
                    success++;
                }
                else{
                    fail++;
                }
            }
            else if(strcmp(argument0, "/topk-AgeRanges") == 0){
                if( topAgeRangesReceive(fd, &readingFds, maxReadingFd, numWorkers, message, bufferSize) ){
                    success++;
                }
                else{
                    fail++;
                }
            }
            else if(strcmp(argument0, "/searchPatientRecord") == 0){
                if( searchByIDReceive(fd, &readingFds, maxReadingFd, numWorkers, message, bufferSize) ){
                    success++;
                }
                else{
                    fail++;
                }
            }
            else if(strcmp(argument0, "/numPatientAdmissions") == 0){
                if( numPatientAdmissionsReceive(fd, &readingFds, maxReadingFd, numWorkers, message, bufferSize) ){
                    success++;
                }
                else{
                    fail++;
                }
            }
            else if(strcmp(argument0, "/numPatientDischarges") == 0){
                if( numPatientDischargesReceive(fd, &readingFds, maxReadingFd, numWorkers, message, bufferSize) ){
                    success++;
                }
                else{
                    fail++;
                }
            }
            else if(strcmp(argument0, "/exit") != 0){
                printf("Wrong Command !!\n");
                fail++;
            }

            if(flag == 1){
                for(i=0; i<numWorkers; i++){
                    kill(childPID[i], SIGKILL);
                }
                break;
            }
        }while(strcmp(argument0, "/exit") != 0);

        printf("exiting\n");

        logfileParent(input_dir, total, success, fail);

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
    }

    free(fd);
    free(childPID);

    return 0;
}
