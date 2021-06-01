#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

int checkRecordID(struct listNode **recordsHashtable, int recordsHashtableSize, char *newRecordID){
    int i, j;
    struct listNode *currentListNode;

    for(i=0; i<recordsHashtableSize; i++){
        if(recordsHashtable[i] != NULL){
            currentListNode = recordsHashtable[i];
            while(currentListNode!=NULL){
                if(strcmp(currentListNode->record->recordID, newRecordID) == 0){
                    printf("Record Exists!!\n");
                    return -1;
                }
    			currentListNode = currentListNode->nextRecord;
    		}
        }
    }

    return 0;
}


int recordsHash(struct listNode **Hash, int recordsHashtableSize, char newRecord[7][20]){
    int position, checkDate;
    struct listNode *current;
    struct date date1, date2;

    checkDate = 1;
    getdate(&(date1), newRecord[5]);

    if(strcmp(newRecord[6], "-")){
        getdate(&(date2), newRecord[6]);
        checkDate = datecmp(date1, date2);
    }

    if(checkDate == -1){
        return 0;
    }

    position = atoi(newRecord[0]) % recordsHashtableSize;

	current=Hash[position];
	if(current==NULL){
		Hash[position]=malloc(sizeof(struct listNode));
        Hash[position]->record = malloc(sizeof(struct record));
		Hash[position]->nextRecord=NULL;
        Hash[position]->record->recordID = malloc( (strlen(newRecord[0]) + 1) * sizeof(char));
		strcpy(Hash[position]->record->recordID, newRecord[0]);
        Hash[position]->record->patientFirstName = malloc( (strlen(newRecord[1]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->patientFirstName, newRecord[1]);
        Hash[position]->record->patientLastName = malloc( (strlen(newRecord[2]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->patientLastName, newRecord[2]);
        Hash[position]->record->diseaseID = malloc( (strlen(newRecord[3]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->diseaseID, newRecord[3]);
        Hash[position]->record->country = malloc( (strlen(newRecord[4]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->country, newRecord[4]);
        Hash[position]->record->entryDate = malloc( (strlen(newRecord[5]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->entryDate, newRecord[5]);
        Hash[position]->record->exitDate = malloc( (strlen(newRecord[6]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->exitDate, newRecord[6]);
	}else{
		while(current->nextRecord!=NULL){
			current = current->nextRecord;
		}
        current->nextRecord = malloc(sizeof(struct listNode));
        current->nextRecord->record = malloc(sizeof(struct record));
		current->nextRecord->nextRecord=NULL;
        current->nextRecord->record->recordID = malloc( (strlen(newRecord[0]) + 1) * sizeof(char));
		strcpy(current->nextRecord->record->recordID, newRecord[0]);
        current->nextRecord->record->patientFirstName = malloc( (strlen(newRecord[1]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->patientFirstName, newRecord[1]);
        current->nextRecord->record->patientLastName = malloc( (strlen(newRecord[2]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->patientLastName, newRecord[2]);
        current->nextRecord->record->diseaseID = malloc( (strlen(newRecord[3]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->diseaseID, newRecord[3]);
        current->nextRecord->record->country = malloc( (strlen(newRecord[4]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->country, newRecord[4]);
        current->nextRecord->record->entryDate = malloc( (strlen(newRecord[5]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->entryDate, newRecord[5]);
        current->nextRecord->record->exitDate = malloc( (strlen(newRecord[6]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->exitDate, newRecord[6]);
	}

    return 1;
}

void Hash(struct bucket **Hash, int HashtableSize, char *key, int entriesPerBucket, struct record *patient){
    int position, i, j;
    struct bucket *current;
    struct date date1;

    getdate(&(date1), patient->entryDate);
    //position = 0;
    //for(i=0; i<strlen(key); i++){
        //position = key[0] + key[strlen(key)-1];
    //}

    position = hashFunction(key, HashtableSize);
	current=Hash[position];

    while(current!=NULL){
        for(i=0; i<entriesPerBucket; i++){
            if(strcmp(key, current->keys[i]) == 0){
                insertTree(&(current->values[i]), patient, date1);
                return;
            }
        }
        current = current->nextBucket;
    }

    current=Hash[position];
	if(current==NULL){
		Hash[position]=malloc(sizeof(struct bucket));
        Hash[position]->nextBucket = NULL;
        Hash[position]->keys = malloc(entriesPerBucket*sizeof(char*));
        Hash[position]->values = malloc(entriesPerBucket*sizeof(struct treeNode*));
        for(j=0; j<entriesPerBucket; j++){
            Hash[position]->keys[j] = malloc(10*sizeof(char));
            Hash[position]->values[j] = NULL;
        }
		strcpy(Hash[position]->keys[0], key);
        insertTree(&(Hash[position]->values[0]), patient, date1);
	}else{
        while(current->nextBucket!=NULL){
			current = current->nextBucket;
		}

        for(i=0; i<entriesPerBucket; i++){
            if(strcmp(current->keys[i], "") == 0){
                break;
            }
        }

        if(i<entriesPerBucket){
            strcpy(current->keys[i], key);
            insertTree(&(current->values[i]), patient, date1);
        }
        else{
            current->nextBucket = malloc(sizeof(struct bucket));
            current->nextBucket->nextBucket = NULL;
            current->nextBucket->keys = malloc(entriesPerBucket*sizeof(char*));
            current->nextBucket->values = malloc(entriesPerBucket*sizeof(struct treeNode*));
            for(j=0; j<entriesPerBucket; j++){
                current->nextBucket->keys[j] = malloc(10*sizeof(char));
                current->nextBucket->values[j] = NULL;
            }
    		strcpy(current->nextBucket->keys[0], key);
            insertTree(&(current->nextBucket->values[0]), patient, date1);
        }
	}

    return;
}

int hashFunction(char *key, int HashtableSize){
    unsigned int i;
    unsigned long int hash;
    i = 0;
    hash = 0;
    while(i != strlen(key)-1){
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash%HashtableSize;
}
