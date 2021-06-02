#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"

int checkRecordID(struct listNode **recordsHashtable, int recordsHashtableSize, char *newRecordID, char *newRecordCondition, char *fileName){
    int i, j, position;
    struct listNode *currentListNode;

    position = atoi(newRecordID) % recordsHashtableSize;
    if(recordsHashtable[position] != NULL){
        currentListNode = recordsHashtable[position];
        while(currentListNode!=NULL){
            if(strcmp(currentListNode->record->recordID, newRecordID)==0 && strcmp(newRecordCondition, "ENTER")==0){
                //printf("Record Exists!! %s %s\n", newRecordID, newRecordCondition);
                return -1;
            }
			currentListNode = currentListNode->nextRecord;
		}
    }

    return 0;
}

int recordsHash(struct listNode **Hash, int recordsHashtableSize, char newRecord[8][20]){
    int position, checkDate;
    struct listNode *current;
    struct date date1, date2;

    if(strcmp(newRecord[1], "EXIT") == 0){        //update record exit date
        position = atoi(newRecord[0]) % recordsHashtableSize;
        current = Hash[position];
        while(current != NULL){
            if(strcmp(current->record->recordID, newRecord[0]) == 0){
                getdate(&date1, current->record->entryDate);
                getdate(&date2, newRecord[7]);
                if(strcmp(current->record->exitDate, "--")!=0){
                    return -1;  //Record's exit date already updated
                }
                if(datecmp(date1, date2) == -1){
                    return -1;  //Exit date %s is before entry date
                }

                strcpy(current->record->exitDate, newRecord[7]);
                return 1;
            }

            current = current->nextRecord;
        }

        return -1;  //Record hasn't been hashed yet
    }
    position = atoi(newRecord[0]) % recordsHashtableSize;

	current=Hash[position];
	if(current==NULL){
		Hash[position]=malloc(sizeof(struct listNode));
        Hash[position]->record = malloc(sizeof(struct record));
		Hash[position]->nextRecord=NULL;
        Hash[position]->record->recordID = malloc( (strlen(newRecord[0]) + 1) * sizeof(char));
		strcpy(Hash[position]->record->recordID, newRecord[0]);
        Hash[position]->record->condition = malloc( (strlen(newRecord[1]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->condition, newRecord[1]);
        Hash[position]->record->patientFirstName = malloc( (strlen(newRecord[2]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->patientFirstName, newRecord[2]);
        Hash[position]->record->patientLastName = malloc( (strlen(newRecord[3]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->patientLastName, newRecord[3]);
        Hash[position]->record->disease = malloc( (strlen(newRecord[4]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->disease, newRecord[4]);
        Hash[position]->record->age = atoi(newRecord[5]);
        Hash[position]->record->country = malloc( (strlen(newRecord[6]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->country, newRecord[6]);
        Hash[position]->record->entryDate = malloc( (strlen(newRecord[7]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->entryDate, newRecord[7]);
        Hash[position]->record->exitDate = malloc( (strlen(newRecord[7]) + 1) * sizeof(char));
        strcpy(Hash[position]->record->exitDate, "--");
	}else{
		while(current->nextRecord!=NULL){
			current = current->nextRecord;
		}

        current->nextRecord = malloc(sizeof(struct listNode));
        current->nextRecord->record = malloc(sizeof(struct record));
		current->nextRecord->nextRecord=NULL;
        current->nextRecord->record->recordID = malloc( (strlen(newRecord[0]) + 1) * sizeof(char));
		strcpy(current->nextRecord->record->recordID, newRecord[0]);
        current->nextRecord->record->condition = malloc( (strlen(newRecord[1]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->condition, newRecord[1]);
        current->nextRecord->record->patientFirstName = malloc( (strlen(newRecord[2]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->patientFirstName, newRecord[2]);
        current->nextRecord->record->patientLastName = malloc( (strlen(newRecord[3]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->patientLastName, newRecord[3]);
        current->nextRecord->record->disease = malloc( (strlen(newRecord[4]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->disease, newRecord[4]);
        current->nextRecord->record->age = atoi(newRecord[5]);
        current->nextRecord->record->country = malloc( (strlen(newRecord[6]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->country, newRecord[6]);
        current->nextRecord->record->entryDate = malloc( (strlen(newRecord[7]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->entryDate, newRecord[7]);
        current->nextRecord->record->exitDate = malloc( (strlen(newRecord[7]) + 1) * sizeof(char));
        strcpy(current->nextRecord->record->exitDate, "--");
	}
    return 1;
}

void Hash(struct bucket **Hash, int HashtableSize, char *key, int entriesPerBucket, struct record *patient){
    int position, i, j;
    struct bucket *current;
    struct date date1;

    getdate(&(date1), patient->entryDate);

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
