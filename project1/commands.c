#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

void globalDiseaseStats(struct bucket **diseaseHashtable, int diseaseHashtableSize, int entriesPerBucket, char *d1, char *d2){
    int i, j, counter;
    struct bucket *current;
    struct date date1, date2;

    getdate(&date1, d1);
    getdate(&date2, d2);

    for(i=0; i<diseaseHashtableSize; i++){
        current = diseaseHashtable[i];
        while(current != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(current->keys[j], "") == 0){
                    continue;
                }
                //printf("Patients with %s: ", current->keys[j]);
                //printf("%s: ", current->keys[j]);
                counter = 0;
                countTreeNodes(current->values[j], &counter, date1, date2);
                //printf("%d\n", counter);
                printf("%s %d\n", current->keys[j], counter);
            }
			current = current->nextBucket;
		}
    }
}

void diseaseFrequency(struct bucket **diseaseHashtable, int diseaseHashtableSize, int entriesPerBucket, char *disease, char *d1, char *d2, char *country){
    int i, j, counter;
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
                //printf("Patients with %s ", currentBucket->keys[j]);
                //printf("%s ", currentBucket->keys[j]);
                counter = 0;
                if(strcmp(country, "") == 0){
                    countTreeNodes(currentBucket->values[j], &counter, date1, date2);
                }
                else{
                    //printf("in %s", country);
                    countTreeNodesInCountry(currentBucket->values[j], &counter, date1, date2, country);
                }
                //printf("%d\n", counter);
                printf("%s %d\n", currentBucket->keys[j], counter);
            }
			currentBucket = currentBucket->nextBucket;
		}
    }
}

void topDiseases(struct bucket **diseaseHashtable, int diseaseHashtableSize, int entriesPerBucket, int k, char *country, char *d1, char *d2){
    int i, j, counter, changes;
    struct bucket *currentBucket;
    struct date date1, date2;
    struct heapNode *heap, *topHeapNode;

    heap = NULL;

    getdate(&date1, d1);
    getdate(&date2, d2);

    for(i=0; i<diseaseHashtableSize; i++){
        currentBucket = diseaseHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(currentBucket->keys[j], "") == 0){
                    continue;
                }

                counter = 0;
                countTreeNodesInCountry(currentBucket->values[j], &counter, date1, date2, country);
                insertHeap(&heap, counter, currentBucket->keys[j]);
            }
			currentBucket = currentBucket->nextBucket;
		}
    }

    do{
        changes = 0;
        heapify(&heap, &changes);
    }while(changes != 0);

    for(i=0; i<k; i++){
        topHeapNode = top(&heap);
        if(topHeapNode->value <= 0){
            return;
        }
        printf("%s %d\n", topHeapNode->key, topHeapNode->value);
        do{
            changes = 0;
            heapify(&heap, &changes);
        }while(changes != 0);
    }

    freeHeap(heap);
}

void topCountries(struct bucket **countryHashtable, int countryHashtableSize, int entriesPerBucket, int k, char *disease, char *d1, char *d2){
    int i, j, counter, changes;
    struct bucket *currentBucket;
    struct date date1, date2;
    struct heapNode *heap, *topHeapNode;

    heap = NULL;

    getdate(&date1, d1);
    getdate(&date2, d2);

    for(i=0; i<countryHashtableSize; i++){
        currentBucket = countryHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if(strcmp(currentBucket->keys[j], "") == 0){
                    continue;
                }

                counter = 0;
                countTreeNodesWithDisease(currentBucket->values[j], &counter, date1, date2, disease);
                insertHeap(&heap, counter, currentBucket->keys[j]);
            }
			currentBucket = currentBucket->nextBucket;
		}
    }

    do{
        changes = 0;
        heapify(&heap, &changes);
    }while(changes != 0);

    for(i=0; i<k; i++){
        topHeapNode = top(&heap);
        if(topHeapNode->value <= 0){
            return;
        }
        printf("%s %d\n", topHeapNode->key, topHeapNode->value);
        do{
            changes = 0;
            heapify(&heap, &changes);
        }while(changes != 0);
    }

    freeHeap(heap);
}

struct record * searchByID(struct listNode **recordsHashtable, int recordsHashtableSize, char *ID){
    int i;
    struct listNode *currentRecord;

    for(i=0; i<recordsHashtableSize; i++){
        currentRecord = recordsHashtable[i];
        while(currentRecord != NULL){
            if(strcmp(currentRecord->record->recordID, ID) == 0){
                return currentRecord->record;
            }
            currentRecord = currentRecord->nextRecord;
        }
    }

    return NULL;
}

void currentPatients(struct bucket **diseaseHashtable, int diseaseHashtableSize, char *disease, int entriesPerBucket){
    int i, j, counter;
    struct bucket *currentBucket;

    for(i=0; i<diseaseHashtableSize; i++){
        currentBucket = diseaseHashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                if((strcmp(disease, "") != 0 && strcmp(disease, currentBucket->keys[j]) != 0) || strcmp(currentBucket->keys[j], "") == 0){
                    continue;
                }
                //printf("Number of current Patients with %s: ", currentBucket->keys[j]);
                counter = 0;
                countTreeNodesWithoutExitDate(currentBucket->values[j], &counter);
                //printf("%d\n", counter);
                printf("%s %d\n", currentBucket->keys[j], counter);
            }
			currentBucket = currentBucket->nextBucket;
		}
    }
}

void countTreeNodesWithoutExitDate(struct treeNode *root, int *counter){
    struct listNode *currentListNode;
    if(root == NULL){
        return;
    }

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        if(strcmp(currentListNode->record->exitDate, "-") == 0){
            (*counter)++;
        }
        currentListNode = currentListNode->nextRecord;
    }

    countTreeNodesWithoutExitDate(root->leftChild, counter);
    countTreeNodesWithoutExitDate(root->rightChild, counter);
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

void countTreeNodesWithDisease(struct treeNode *root, int *counter, struct date date1, struct date date2, char *disease){
    struct listNode *currentListNode;
    if(root == NULL){
        return;
    }

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        if(datecmp(root->date, date1) <=0 && datecmp(root->date, date2) >=0){
            if(strcmp(currentListNode->record->diseaseID, disease) == 0){
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
