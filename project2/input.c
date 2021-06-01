#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"

void saveRecords(struct dateFileNode *files, struct listNode **recordsHashtable, int recordsHashtableSize, char input_dir[64], char **workerCountries, int countriesPerWorker){
    int i, filesPerCountry;
    char path[64], **dateFiles;
    DIR *dir, *sdir;
    FILE *fd;
    struct dirent *subDir, *file;
    struct record newRecord;
    struct dateFileNode *currentListNode;

    if((dir=opendir(input_dir)) == NULL){           //open input directory
        printf("Error in opening Directory\n");
        return;
    }

    while((subDir=readdir(dir)) != NULL){
        if(strcmp(subDir->d_name, ".") == 0 || strcmp(subDir->d_name, "..") == 0){
            continue;
        }
        if(subDir->d_type == DT_DIR && countryOfWorker(workerCountries, countriesPerWorker, subDir->d_name)){
            strcpy(path, input_dir);
            strncat(path, "/", 1);
            strncat(path, subDir->d_name, strlen(subDir->d_name));

            if((sdir=opendir(path)) == NULL){                       //open each subdirectory
                printf("Error in opening subdirectory\n");
                return;
            }

            filesPerCountry = 0;
            while((file=readdir(sdir)) != NULL){
                if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                    continue;
                }

                if(file->d_type == DT_REG){
                    filesPerCountry++;
                }
            }
            dateFiles = malloc(filesPerCountry*sizeof(char *));

            closedir(sdir);
            if((sdir=opendir(path)) == NULL){                       //open each subdirectory
                printf("Error in opening subdirectory\n");
                return;
            }

            i=0;
            while((file=readdir(sdir)) != NULL){
                if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                    continue;
                }

                if(file->d_type == DT_REG){
                    dateFiles[i] = malloc( (strlen(file->d_name)+1) * sizeof(char));
                    strcpy(dateFiles[i], file->d_name);
                    i++;
                }
            }

            sortDateFiles(dateFiles, filesPerCountry);

            for(i=0; i<filesPerCountry; i++){
                strncat(path, "/", 1);
                strncat(path, dateFiles[i], strlen(dateFiles[i]));
                if((fd=fopen(path, "r")) == NULL){              //open each file
                    printf("Error in opening file\n");
                    return;
                }

                currentListNode = files;
                while(currentListNode != NULL){
                    currentListNode = currentListNode->nextDateFileNode;
                }
                currentListNode = malloc(sizeof(struct dateFileNode));
                currentListNode->fileName = malloc( (strlen(dateFiles[i])+1) * sizeof(char));
                strcpy(currentListNode->fileName, dateFiles[i]);
                currentListNode->nextDateFileNode = NULL;

                readRecords(subDir->d_name, dateFiles[i], fd, recordsHashtable, recordsHashtableSize);
                fclose(fd);
                strcpy(path, input_dir);
                strncat(path, "/", 1);
                strncat(path, subDir->d_name, strlen(subDir->d_name));
            }

            free(dateFiles);
            closedir(sdir);
        }
    }

    closedir(dir);
}

void readRecords(char *subDirName, char *fileName, FILE *fd, struct listNode **recordsHashtable, int recordsHashtableSize){
    char ch, newRecord[8][20];
    int i, j;

    ch = fgetc(fd);
    while(ch != EOF){
        i=0;
        while(ch != '\n' ){
            j=0;
            while (ch != ' ' && ch != '\t' && ch != '\n' ){
                newRecord[i][j] = ch;
                j++;
                ch = fgetc(fd);
            }
            newRecord[i][j] = '\0';

            if(ch != '\n' ){
                ch = fgetc(fd);
            }
            i++;
        }

        strcpy(newRecord[6], subDirName);
        strcpy(newRecord[7], fileName);

        if(checkRecordID(recordsHashtable, recordsHashtableSize, newRecord[0], newRecord[1], fileName) == 0){
            recordsHash(recordsHashtable, recordsHashtableSize, newRecord);
        }

        ch = fgetc(fd);
    }
}

int countCountries(char input_dir[64]){
    int numCountries;
    DIR *dir;
    struct dirent *subDir;

    if((dir=opendir(input_dir)) == NULL){           //open input directory
        perror("Error in opening Directory");
        return 0;
    }

    numCountries = 0;
    while((subDir=readdir(dir)) != NULL){
        if(strcmp(subDir->d_name, ".") == 0 || strcmp(subDir->d_name, "..") == 0){
            continue;
        }
        if(subDir->d_type == DT_DIR){
            numCountries++;
        }
    }

    closedir(dir);
    return numCountries;
}

void saveCountries(char input_dir[64], char **countries){
    int i;
    DIR *dir;
    struct dirent *subDir;

    if((dir=opendir(input_dir)) == NULL){           //open input directory
        perror("Error in opening Directory");
        return;
    }

    i = 0;
    while((subDir=readdir(dir)) != NULL){
        if(strcmp(subDir->d_name, ".") == 0 || strcmp(subDir->d_name, "..") == 0){
            continue;
        }
        if(subDir->d_type == DT_DIR){
            countries[i] = malloc((strlen(subDir->d_name) + 1) * sizeof(char) );
            strcpy(countries[i], subDir->d_name);
            i++;
        }
    }

    closedir(dir);
}

int countryOfWorker(char **workerCountries, int countriesPerWorker, char *country){     //returns 1 if country belongs to wrorker
    int i;                                                                              //returns 0 if not

    for(i=0; i<countriesPerWorker; i++){
        if(strcmp(workerCountries[i], country) == 0){
            return 1;
        }
    }

    return 0;
}

void sortDateFiles(char **dateFiles, int filesPerCountry){
    int i, j;
    char temp[11];
    struct date date1, date2;

    for(i=0; i<filesPerCountry-1; i++){
        for(j=0; j<filesPerCountry-i-1; j++){
            getdate(&date1, dateFiles[j]);
            getdate(&date2, dateFiles[j+1]);
            if(datecmp(date1, date2) == -1){        //swap
                strcpy(temp, dateFiles[j]);
                strcpy(dateFiles[j], dateFiles[j+1]);
                strcpy(dateFiles[j+1], temp);
            }
        }
    }
}
