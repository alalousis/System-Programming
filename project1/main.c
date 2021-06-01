#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"

int main(int argc, char *argv[]){
    int i, j, z, k, l, recordsHashtableSize, diseaseHashtableSize, countryHashtableSize, bucketSize, entriesPerBucket;
    char ch, input[64], query[128], argument[8][32], newRecord[7][20];
    struct listNode **recordsHashtable, *currentListNode;
    struct bucket **diseaseHashtable, **countryHashtable;
    struct record *patient;
    struct date date1, date2;
    FILE *fd;

    for(i=1; i<argc-1; i++){
        if(strcmp(argv[i], "-p") == 0){
                strcpy(input, argv[i+1]);
        }
        else if(strcmp(argv[i], "-h1") == 0){
                diseaseHashtableSize = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-h2") == 0){
                countryHashtableSize = atoi(argv[i+1]);
        }
        else if(strcmp(argv[i], "-b") == 0){
                bucketSize = atoi(argv[i+1]);
        }
    }

    entriesPerBucket = (bucketSize - 8) / 16;
    recordsHashtableSize = 10;

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

    fd = fopen(input, "r");
    if(saveRecords(fd, recordsHashtable, recordsHashtableSize) == -1){
        printf("Duplicate Entry Error\n");
    }

    for(i=0; i<recordsHashtableSize; i++){
        currentListNode = recordsHashtable[i];
        while(currentListNode != NULL){
            Hash(diseaseHashtable, diseaseHashtableSize, currentListNode->record->diseaseID, entriesPerBucket, currentListNode->record);
            Hash(countryHashtable, countryHashtableSize, currentListNode->record->country, entriesPerBucket, currentListNode->record);
			currentListNode = currentListNode->nextRecord;
		}
    }

    do{
        //printf("\nPlease give command:\n");
        for(k=0; k<8; k++){
            for(l=0; l<32; l++){
                argument[k][l] = 0;
            }
        }
        for(k=0; k<128; k++){
            query[k] = 0;
        }
        scanf(" %[^\n]s", query);
        i=0;
        j=0;
        ch = query[0];
        while(ch != 0){
            z=0;
            while(ch != 32 && ch != 0){
                argument[j][z] = ch;
                z++;
                i++;
                ch = query[i];
            }
            argument[j][z] = '\0';
            j++;
            i++;
            ch = query[i];
        }

        if(strcmp(argument[0], "/globalDiseaseStats") == 0){
            if(strcmp(argument[1], "") != 0){
                if(strcmp(argument[2], "") == 0){
                    printf("Date2 not inserted");
                    continue;
                }
            }
            else{
                strcpy(argument[1], "01-01-1900");
                strcpy(argument[2], "31-12-3000");
            }
            globalDiseaseStats(diseaseHashtable, diseaseHashtableSize, entriesPerBucket, argument[1], argument[2]);
        }
        else if(strcmp(argument[0], "/diseaseFrequency") == 0){
            diseaseFrequency(diseaseHashtable, diseaseHashtableSize, entriesPerBucket, argument[1], argument[2],  argument[3],  argument[4]);
        }
        else if(strcmp(argument[0], "/topk-Diseases") == 0){
            if(strcmp(argument[3], "") != 0){
                if(strcmp(argument[4], "") == 0){
                    printf("Date2 not inserted");
                    continue;
                }
            }
            else{
                strcpy(argument[3], "01-01-1900");
                strcpy(argument[4], "31-12-3000");
            }
            topDiseases(diseaseHashtable, diseaseHashtableSize, entriesPerBucket, atoi(argument[1]), argument[2],  argument[3],  argument[4]);
        }
        else if(strcmp(argument[0], "/topk-Countries") == 0){
            if(strcmp(argument[3], "") != 0){
                if(strcmp(argument[4], "") == 0){
                    printf("Date2 not inserted");
                    continue;
                }
            }
            else{
                strcpy(argument[3], "01-01-1900");
                strcpy(argument[4], "31-12-3000");
            }
            topCountries(countryHashtable, countryHashtableSize, entriesPerBucket, atoi(argument[1]), argument[2],  argument[3],  argument[4]);
        }
        else if(strcmp(argument[0], "/insertPatientRecord") == 0){
            if(strcmp(argument[7], "") == 0){
                strcpy(argument[7], "-");
            }

            for(i=0; i<7; i++){
                strcpy(newRecord[i], argument[i+1]);
            }

            if(checkRecordID(recordsHashtable, recordsHashtableSize, newRecord[0]) == -1){
                break;
            }

            if(recordsHash(recordsHashtable, recordsHashtableSize, newRecord) == 1){
                patient = searchByID(recordsHashtable, recordsHashtableSize, newRecord[0]);
                Hash(diseaseHashtable, diseaseHashtableSize, newRecord[3], entriesPerBucket, patient);
                Hash(countryHashtable, countryHashtableSize, newRecord[4], entriesPerBucket, patient);
            }
            printf("Record added\n");
        }
        else if(strcmp(argument[0], "/recordPatientExit") == 0){
            patient = searchByID(recordsHashtable, recordsHashtableSize, argument[1]);
            if(patient == NULL){
                printf("Not found\n");
            }
            getdate(&date1, patient->entryDate);
            getdate(&date2, argument[2]);
            if(datecmp(date1, date2) == -1){                                //date1 > date2
                printf("Error: Exit-date is before entry-date!\n");
            }
            else{
                strcpy(patient->exitDate, argument[2]);
                printf("Record updated\n");
            }
        }
        else if(strcmp(argument[0], "/numCurrentPatients") == 0){
            currentPatients(diseaseHashtable, diseaseHashtableSize, argument[1], entriesPerBucket);
        }
        else if(strcmp(argument[0], "/exit") != 0){
            printf("Wrong Command !!\n");
        }
    }while(strcmp(argument[0], "/exit") != 0);

    printf("exiting\n");

    fclose(fd);
    freeRecordsHashTable(recordsHashtable, recordsHashtableSize);
    freeHashTable(diseaseHashtable, diseaseHashtableSize, entriesPerBucket);
    freeHashTable(countryHashtable, countryHashtableSize, entriesPerBucket);
    return 0;
}
