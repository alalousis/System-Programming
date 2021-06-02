#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

int countQueries(char *fileName){
    int numQueries, lineSize;
    char ch;
    FILE *fd;

    numQueries = 0;
    if((fd=fopen(fileName, "r")) == NULL){
        perror("Error in opening file");
        return 0;
    }

    ch = fgetc(fd);
    while(ch != EOF){
        if(ch == '/'){
            numQueries++;
        }
        ch = fgetc(fd);
    }

    return numQueries;
}

void readQueries(char *fileName, char **queries, int numQueries){
    int i, lineCounter;
    ssize_t querySize = 100;
    size_t lineSize;
    FILE *fd;

    if((fd=fopen(fileName, "r")) == NULL){
        perror("Error in opening file");
        return;
    }

    for(i=0; i<numQueries; i++){
        lineSize = getline(&queries[i], &querySize, fd);
        queries[i][lineSize-1] = '\0';
    }

    fclose(fd);
}
