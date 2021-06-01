#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "structs.h"
#include "functions.h"

void listCountries(char **workerCountries, int countriesPerWorker, char *message, int fdWrite){
    int i, messageSize;
    char pid[8];

    message[0] = '\0';
    sprintf(pid, "%d", getpid());
    strncat(message, pid, strlen(pid));
    strncat(message, "#", 1);

    for(i=0; i<countriesPerWorker; i++){
        strncat(message, workerCountries[i], strlen(workerCountries[i]));
        strncat(message, "#", 1);
    }
    strncat(message, "$", 1);
    messageSize = strlen(message);

    if(write(fdWrite, message, messageSize) != messageSize){
        perror("error in writing message");
    }
}

void diseaseFrequency(struct bucket **diseaseHashtable, int diseaseHashtableSize, int entriesPerBucket, char *disease, char *d1, char *d2, char *country, char *message, int fdWrite){
    int i, j, counter, messageSize;
    char result[4];
    struct bucket *currentBucket;
    struct date date1, date2;

    getdate(&date1, d1);
    getdate(&date2, d2);

    for(i=0; i<diseaseHashtableSize; i++){
        currentBucket = diseaseHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(currentBucket->keys[j], "") == 0 || strcmp(currentBucket->keys[j], disease) != 0){
                    continue;
                }

                counter = 0;
                if(strcmp(country, "") == 0){
                    countTreeNodes(currentBucket->values[j], &counter, date1, date2);
                }
                else{
                    countTreeNodesInCountry(currentBucket->values[j], &counter, date1, date2, country);
                }
            }
			currentBucket = currentBucket->nextBucket;
		}
    }

    message[0] = '\0';
    result[0] = '\0';
    sprintf(result, "%d", counter);;
    strncat(message, result, strlen(result));
    messageSize = strlen(message);
    message[messageSize] = '\0';
    if(write(fdWrite, message, messageSize) != messageSize){
        perror("error in writing message");
    }
}

void topAgeRanges(struct bucket **diseaseHashtable, int diseaseHashtableSize, int entriesPerBucket, int k, char *disease, char *country, char *d1, char *d2, char *message, int fdWrite){
    int i, j, position, totalCounter, changes, messageSize;
    float perchentage;
    int counters[4];
    char ranges[4][8], ch, result[8];
    struct bucket *currentBucket;
    struct date date1, date2;
    struct heapNode *heap, *topHeapNode;

    for(i=0; i<4; i++){
        counters[i] = 0;
    }
    strcpy(ranges[0], "0-20");
    strcpy(ranges[1], "21-40");
    strcpy(ranges[2], "41-60");
    strcpy(ranges[3], "60+");
    heap = NULL;

    getdate(&date1, d1);
    getdate(&date2, d2);

    position = hashFunction(disease, diseaseHashtableSize);
    currentBucket = diseaseHashtable[position];
    while(currentBucket != NULL){
        for(j=0; j<entriesPerBucket; j++){
            if(strcmp(currentBucket->keys[j], "") == 0){
                continue;
            }

            if(strcmp(currentBucket->keys[j], disease) == 0){
                totalCounter = 0;
                countTreeNodesInCountry(currentBucket->values[j], &totalCounter, date1, date2, country);

                countTreeNodesInCountryInRange(currentBucket->values[j], counters, date1, date2, country);
                for(i=0; i<4; i++){
                    insertHeap(&heap, counters[i], ranges[i]);
                }

                break;
            }
        }
		currentBucket = currentBucket->nextBucket;
	}


    do{
        changes = 0;
        heapify(&heap, &changes);
    }while(changes != 0);

    strcpy(message, "");
    strncat(message, "y", 1);
    strncat(message, "#", 1);

    for(i=0; i<k; i++){
        topHeapNode = top(&heap);
        if(topHeapNode->value < 0){
            return;
        }
        perchentage = (float) topHeapNode->value / totalCounter * 100.0;

        ch = topHeapNode->key[0];
        strncat(message, &ch, 1);
        sprintf(result, "%.2f", perchentage);
        strncat(message, result, strlen(result));
        strncat(message, "#", 1);
        do{
            changes = 0;
            heapify(&heap, &changes);
        }while(changes != 0);
    }

    strncat(message, "$", 1);
    messageSize = strlen(message);
    if(write(fdWrite, message, messageSize) != messageSize){
        perror("error in writing message");
    }

    //freeHeap(heap);
}

void searchByID(struct listNode **recordsHashtable, int recordsHashtableSize, char *ID, char *message, int fdWrite){
    int position, messageSize;
    char age[4];
    struct listNode *currentRecord;

    position = atoi(ID) % recordsHashtableSize;
    currentRecord = recordsHashtable[position];
    while(currentRecord != NULL){
        if(strcmp(currentRecord->record->recordID, ID) == 0){
            strncat(message, currentRecord->record->recordID, strlen(currentRecord->record->recordID));
            strncat(message, "#", 1);

            strncat(message, currentRecord->record->patientFirstName, strlen(currentRecord->record->patientFirstName));
            strncat(message, "#", 1);

            strncat(message, currentRecord->record->patientLastName, strlen(currentRecord->record->patientLastName));
            strncat(message, "#", 1);

            strncat(message, currentRecord->record->disease, strlen(currentRecord->record->disease));
            strncat(message, "#", 1);

            sprintf(age, "%d", currentRecord->record->age);
            strncat(message, age, strlen(age));
            strncat(message, "#", 1);

            strncat(message, currentRecord->record->entryDate, strlen(currentRecord->record->entryDate));
            strncat(message, "#", 1);

            strncat(message, currentRecord->record->exitDate, strlen(currentRecord->record->exitDate));
            strncat(message, "#", 1);
            break;
        }
        currentRecord = currentRecord->nextRecord;
    }
    strncat(message, "$", 1);
    messageSize = strlen(message);
    message[messageSize] = '\0';

    if(write(fdWrite, message, messageSize) != messageSize){
        perror("error in writing message");
    }
}

void numPatientAdmissions(struct bucket **countryHashtable, int countryHashtableSize, int entriesPerBucket, char *disease, char *d1, char *d2, char *country, char *message, int fdWrite){
    int i, j, counter, messageSize;
    char result[4];
    struct bucket *currentBucket;
    struct date date1, date2;

    getdate(&date1, d1);
    getdate(&date2, d2);

    for(i=0; i<countryHashtableSize; i++){
        currentBucket = countryHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(currentBucket->keys[j], "") == 0){
                    continue;
                }
                else if(currentBucket->keys[j][0]<65 || currentBucket->keys[j][0]>90){
                    continue;
                }

                if(strcmp(country, "") != 0 && strcmp(currentBucket->keys[j], country) != 0){
                    continue;
                }
                counter = 0;

                countTreeNodesWithDisease(currentBucket->values[j], &counter, date1, date2, disease);

                strncat(message, currentBucket->keys[j], strlen(currentBucket->keys[j]));
                strncat(message, "#", 1);
                result[0] = '\0';
                sprintf(result, "%d", counter);;
                strncat(message, result, strlen(result));
                strncat(message, "/", 1);
            }
			currentBucket = currentBucket->nextBucket;
		}
    }

    strncat(message, "$", 1);
    messageSize = strlen(message);
    message[messageSize] = '\0';
    if(write(fdWrite, message, messageSize) != messageSize){
        perror("error in writing message");
    }
}

void numPatientDischarges(struct bucket **countryHashtable, int countryHashtableSize, int entriesPerBucket, char *disease, char *d1, char *d2, char *country, char *message, int fdWrite){
    int i, j, counter, messageSize;
    char result[4];
    struct bucket *currentBucket;
    struct date date1, date2;

    getdate(&date1, d1);
    getdate(&date2, d2);

    for(i=0; i<countryHashtableSize; i++){
        currentBucket = countryHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(currentBucket->keys[j], "") == 0){
                    continue;
                }

                if(currentBucket->keys[j][0]<65 || currentBucket->keys[j][0]>90){
                    continue;
                }

                if(strcmp(country, "") != 0 && strcmp(currentBucket->keys[j], country) != 0){
                    continue;
                }

                counter = 0;
                countDischargesInCountry(currentBucket->values[j], &counter, date1, date2, disease);

                strncat(message, currentBucket->keys[j], strlen(currentBucket->keys[j]));
                strncat(message, "#", 1);
                result[0] = '\0';
                sprintf(result, "%d", counter);;
                strncat(message, result, strlen(result));
                strncat(message, "/", 1);
            }
			currentBucket = currentBucket->nextBucket;
		}
    }

    strncat(message, "$", 1);
    messageSize = strlen(message);
    message[messageSize] = '\0';
    if(write(fdWrite, message, messageSize) != messageSize){
        perror("error in writing message");
    }
}



void countTreeNodes(struct treeNode *root, int *counter, struct date date1, struct date date2){
    struct listNode *currentListNode;
    if(root == NULL){
        return;
    }

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        if(datecmp(root->date, date1) <= 0 && datecmp(root->date, date2) >= 0){
            (*counter)++;
        }
        currentListNode = currentListNode->nextRecord;
    }

    if(datecmp(root->date, date1) <= 0){
        countTreeNodes(root->leftChild, counter, date1, date2);
    }

    if(datecmp(root->date, date2) >= 0){
        countTreeNodes(root->rightChild, counter, date1, date2);
    }
}

void countTreeNodesInCountry(struct treeNode *root, int *counter, struct date date1, struct date date2, char *country){
    struct listNode *currentListNode;
    if(root == NULL){
        return;
    }

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        if(datecmp(root->date, date1) <=0 && datecmp(root->date, date2) >=0){
            if(strcmp(currentListNode->record->country, country) == 0){
                (*counter)++;
            }
        }
        currentListNode = currentListNode->nextRecord;
    }

    if(datecmp(root->date, date1) <= 0){
        countTreeNodesInCountry(root->leftChild, counter, date1, date2, country);
    }

    if(datecmp(root->date, date2) >= 0){
        countTreeNodesInCountry(root->rightChild, counter, date1, date2, country);
    }
}

void countTreeNodesInCountryInRange(struct treeNode *root, int *counters, struct date date1, struct date date2, char *country){
    struct listNode *currentListNode;

    if(root == NULL){
        return;
    }

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        if(datecmp(root->date, date1) <=0 && datecmp(root->date, date2) >=0){
            if(strcmp(currentListNode->record->country, country) == 0){
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
        }
        currentListNode = currentListNode->nextRecord;
    }

    if(datecmp(root->date, date1) <= 0){
        countTreeNodesInCountryInRange(root->leftChild, counters, date1, date2, country);
    }

    if(datecmp(root->date, date2) >= 0){
        countTreeNodesInCountryInRange(root->rightChild, counters, date1, date2, country);
    }
}

void countDischargesInCountry(struct treeNode *root, int *counter, struct date date1, struct date date2, char *disease){
    struct listNode *currentListNode;
    struct date date0;

    if(root == NULL){
        return;
    }

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        if(strcmp(currentListNode->record->disease, disease) == 0){
            getdate(&date0, currentListNode->record->exitDate);
            if(datecmp(date0, date1) <=0 && datecmp(date0, date2) >=0){
                (*counter)++;
            }
        }
        currentListNode = currentListNode->nextRecord;
    }

    countDischargesInCountry(root->leftChild, counter, date1, date2, disease);
    countDischargesInCountry(root->rightChild, counter, date1, date2, disease);
}

void countTreeNodesWithDisease(struct treeNode *root, int *counter, struct date date1, struct date date2, char *disease){
    struct listNode *currentListNode;
    if(root == NULL){
        return;
    }

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        if(datecmp(root->date, date1) <=0 && datecmp(root->date, date2) >=0){
            if(strcmp(currentListNode->record->disease, disease) == 0){
                (*counter)++;
            }
        }
        currentListNode = currentListNode->nextRecord;
    }

    if(datecmp(root->date, date1) <= 0){
        countTreeNodesWithDisease(root->leftChild, counter, date1, date2, disease);
    }

    if(datecmp(root->date, date2) >= 0){
        countTreeNodesWithDisease(root->rightChild, counter, date1, date2, disease);
    }
}
