#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "structs.h"
#include "functions.h"

void logfileParent(char input_dir[64], int total, int success, int fail){
    int i, numCountries;
    char **countries, logfileName[16], pid[8];
    FILE *fp;

    sprintf(pid, "%d", getpid());
    strcpy(logfileName, "log_file.");
    strncat(logfileName, pid, strlen(pid)+1);
    logfileName[strlen(logfileName)] = '\0';

    if((fp=fopen(logfileName, "w")) == NULL){
        perror("Error in opening logfile");
        return;
    }

    numCountries = countCountries(input_dir);
    countries = malloc(numCountries*sizeof(char *));
    saveCountries(input_dir, countries);

    for(i=0; i<numCountries; i++){
        fprintf(fp, "%s\n", countries[i]);
    }
    fprintf(fp, "TOTAL %d\n", total);
    fprintf(fp, "SUCCESS %d\n", success);
    fprintf(fp, "FAIL %d\n", fail);

    fclose(fp);
}

void logfileChild(char **workerCountries, int countriesPerWorker, int total, int success, int fail){
    int i, numCountries;
    char **countries, logfileName[16], pid[8];
    FILE *fp;

    sprintf(pid, "%d", getpid());
    strcpy(logfileName, "log_file.");
    strncat(logfileName, pid, strlen(pid));
    logfileName[strlen(logfileName)] = '\0';

    if((fp=fopen(logfileName, "w")) == NULL){
        perror("Error in opening logfile");
        return;
    }

    for(i=0; i<countriesPerWorker; i++){
        fprintf(fp, "%s\n", workerCountries[i]);
    }
    fprintf(fp, "TOTAL %d\n", total);
    fprintf(fp, "SUCCESS %d\n", success);
    fprintf(fp, "FAIL %d\n", fail);

    fclose(fp);
}

void sigusr1(struct dateFileNode *files, char input_dir[64], char **workerCountries, int countriesPerWorker, struct listNode **recordsHashtable, int recordsHashtableSize){
    int i, filesPerCountry;
    char path[64], **dateFiles;
    DIR *dir, *sdir;
    FILE *fd;
    struct dirent *subDir, *file;
    struct record newRecord;

    if((dir=opendir(input_dir)) == NULL){           //open input directory
        perror("Error in opening Directory");
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
                perror("Error in opening subdirectory");
                return;
            }

            filesPerCountry = 0;
            while((file=readdir(sdir)) != NULL){
                if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                    continue;
                }

                if(file->d_type == DT_REG && dateOfWorker(files, file->d_name)==0){
                    strncat(path, "/", 1);
                    strncat(path, file->d_name, strlen(file->d_name));
                    if((fd=fopen(path, "r")) == NULL){              //open each file
                        printf("Error in opening file\n");
                        return;
                    }

                    readRecords(subDir->d_name, file->d_name, fd, recordsHashtable, recordsHashtableSize);
                    fclose(fd);
                    strcpy(path, input_dir);
                    strncat(path, "/", 1);
                    strncat(path, subDir->d_name, strlen(subDir->d_name));
                }
            }
            closedir(sdir);
        }
    }

    closedir(dir);
}

int dateOfWorker(struct dateFileNode *files, char *fileName){
    struct dateFileNode *currentListNode;

    currentListNode = files;
    while(currentListNode != NULL){
        if(strcmp(currentListNode->fileName, fileName) == 0){
            return 1;
        }
        currentListNode = currentListNode->nextDateFileNode;
    }

    return 0;
}
