#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "structs.h"
#include "functions.h"

void statisticsSend(struct bucket **countryHashtable, int countryHashtableSize, struct bucket **diseaseHashtable, int diseaseHashtableSize, int entriesPerBucket, char *message, int fdWrite){
    int i, j, counters[4], messageSize;
    struct bucket *currentBucket;

    message[0] = '\0';
    for(i=0; i<countryHashtableSize; i++){
        currentBucket = countryHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(currentBucket->keys[j], "") == 0 ){
                    continue;
                }
                else if(currentBucket->keys[j][0]<65 || currentBucket->keys[j][0]>90){
                    continue;
                }

                strncat(message, currentBucket->keys[j], strlen(currentBucket->keys[j])+1);
                strncat(message, "&", 1);
                messageSize = strlen(message);
                message[messageSize] = '\0';

                countTreeNodesWithDiseaseInRange(currentBucket->values[j], counters, diseaseHashtable, diseaseHashtableSize, entriesPerBucket, message, fdWrite);
                strncat(message, "$", 1);
            }
            currentBucket = currentBucket->nextBucket;
        }
    }

    strncat(message, "*", 1);
    messageSize = strlen(message);
    message[messageSize] = '\0';

    if(write(fdWrite, message, messageSize) != messageSize){
        perror("error in writing message");
    }
}

void statisticsReceive(int **fd, char *message, int bufferSize, int numWorkers){
    int i, j, k, l, m, n, z, messageSize, confirmationSize;
    char country[16], disease[16], date[11], counter[4][4];

    for(i=0; i<numWorkers; i++){
        message[0] = '\0';
        if( (messageSize=read(fd[i][0], message, bufferSize)) <= 0 ){
            perror("error in reading message");
        }
        message[messageSize] = '\0';

        z=0;
        while(message[z] != '*'){
            j=0;
            country[0] = '\0';
            while(message[z] != '&'){
                country[j] = message[z];
                j++;
                z++;
            }
            country[j] = '\0';

            z++;
            while(message[z] != '$'){
                k=0;
                date[0] = '\0';
                while(message[z] != '^'){
                    date[k] = message[z];
                    k++;
                    z++;

                }
                date[k] = '\0';
                z++;
                printf("%s\n", date);
                printf("%s\n", country);
                while(message[z] != '&'){

                    l=0;
                    disease[0] = '\0';
                    while(message[z] != '#'){
                        disease[l] = message[z];
                        l++;
                        z++;
                    }
                    disease[l] = '\0';
                    z++;
                    printf("%s\n", disease);
                    for(n=0; n<4; n++){

                        m=0;
                        counter[n][0] = '\0';
                        while(message[z] != '#' && message[z] != '/'){
                            counter[n][m] = message[z];
                            m++;
                            z++;
                        }
                        counter[n][m] = '\0';
                        z++;
                    }
                    printf("Age range 0-20 years: %s\n", counter[0]);
                    printf("Age range 21-40 years: %s\n", counter[1]);
                    printf("Age range 41-60 years: %s\n", counter[2]);
                    printf("Age range 60+ years: %s\n\n", counter[3]);
                }
                z++;
            }
            z++;
        }
    }
}

void countTreeNodesWithDiseaseInRange(struct treeNode *root, int *counters, struct bucket **diseaseHashtable, int diseaseHashtableSize, int entriesPerBucket, char *message, int fdWrite){
    int i, j, k, z, messageSize;
    char counter[4];
    struct listNode *currentListNode;
    struct bucket *currentBucket;

    if(root == NULL){
        return;
    }

    strncat(message, root->recordsList->record->entryDate, strlen(root->recordsList->record->entryDate) + 1);
    strncat(message, "^", 1);
    messageSize = strlen(message);
    message[messageSize] = '\0';

    for(i=0; i<diseaseHashtableSize; i++){
        currentBucket = diseaseHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(currentBucket->keys[j], "") == 0 ){
                    continue;
                }
                else if(currentBucket->keys[j][0]<65 || currentBucket->keys[j][0]>90){
                    continue;
                }
                strncat(message, currentBucket->keys[j], 10);
                messageSize = strlen(message);
                message[messageSize] = '\0';

                counters[0] = 0;
                counters[1] = 0;
                counters[2] = 0;
                counters[3] = 0;
                currentListNode = root->recordsList;
                while(currentListNode != NULL){
                    if(strcmp(currentListNode->record->disease, currentBucket->keys[j]) == 0){
                        if(currentListNode->record->age <= 20){
                            counters[0]++;
                        }
                        else if(currentListNode->record->age >= 21 && currentListNode->record->age <= 40){
                            counters[1]++;
                        }
                        else if(currentListNode->record->age >= 41 && currentListNode->record->age <= 60){
                            counters[2]++;
                        }
                        else{
                            counters[3]++;
                        }
                    }
                    currentListNode = currentListNode->nextRecord;
                }
                for(k=0; k<4; k++){
                    strncat(message, "#", 1);
                    sprintf(counter, "%d", counters[k]);
                    strncat(message, counter, strlen(counter) + 1);
                    messageSize = strlen(message);
                    message[messageSize] = '\0';
                }
                strncat(message, "/", 1);
                messageSize = strlen(message);
                message[messageSize] = '\0';
            }
            currentBucket = currentBucket->nextBucket;
        }
    }

    strncat(message, "&", 1);
    messageSize = strlen(message);
    message[messageSize] = '\0';


    countTreeNodesWithDiseaseInRange(root->leftChild, counters, diseaseHashtable, diseaseHashtableSize, entriesPerBucket, message, fdWrite);

    countTreeNodesWithDiseaseInRange(root->rightChild, counters, diseaseHashtable, diseaseHashtableSize, entriesPerBucket, message, fdWrite);
}
