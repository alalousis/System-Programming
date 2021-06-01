#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "structs.h"
#include "functions.h"

void assignCountries(char *input_dir, int numWorkers, char *message, int bufferSize, int **fd){
    int i, j, k, numCountries, countriesPerWorker, messageSize;
    char **countries, ch;

    numCountries = countCountries(input_dir);
    countries = malloc(numCountries*sizeof(char *));
    saveCountries(input_dir, countries);

    for(i=0; i<numWorkers; i++){
        countriesPerWorker = numCountries / numWorkers;
        message[0] = '\0';
        if(i < numCountries%numWorkers){
            countriesPerWorker++;
        }

        ch = '0' + countriesPerWorker;
        strncat(message, &ch, 1);

        for(k=0; k<countriesPerWorker; k++){
            for(j=0; j<numCountries; j++){
                if(i == j%numWorkers && j/numWorkers == k){
                    strncat(message, countries[j], strlen(countries[j]));
                    strncat(message, "#", 1);
                }
            }
        }

        messageSize = strlen(message);
        if(write(fd[i][1], message, messageSize) != messageSize){
            perror("error in writing message");
        }
    }
}

void decodeCountries(int fdRead, char *message, int bufferSize, char **workerCountries, int countriesPerWorker){
    int i, j, z;
    char country[16];

    j=1;
    for(i=0; i<countriesPerWorker; i++){
        z=0;
        while(message[j] != '#'){
            country[z] = message[j];
            z++;
            j++;
        }
        country[z] = '\0';
        j++;
        workerCountries[i] = malloc( (strlen(country) + 1) * sizeof(char) );
        strcpy(workerCountries[i], country);
    }
}

void sendCommand(int **fd, fd_set *writingFds, int maxWritingFd, char *query, char *message, int bufferSize, int numWorkers){
    int i, messageSize, worker;

    message[0] = '\0';
    strncat(message, query, strlen(query));
    messageSize = strlen(message);
    message[messageSize] = '\0';

    for(i=0; i<numWorkers; i++){
        if(write(fd[i][1], message, messageSize) != messageSize){
            perror("error in writing message");
        }
    }

}

void receiveCommand(int fdRead, char argument[8][32], char *message, int bufferSize){
    int i, j, k, z, messageSize;
    char ch;

    for(k=0; k<=7; k++){
        strcpy(argument[k], "");
    }

    message[0] = '\0';

    if( (messageSize=read(fdRead, message, bufferSize)) <= 0 ){
        perror("error in reading message");
    }

    message[messageSize] = '\0';

    j=0;
    z=0;
    for(i=0; i<messageSize; i++){
        if(message[i] == ' '){
            argument[j][z] = '\0';
            j++;
            i++;
            z=0;
        }
        argument[j][z] = message[i];
        z++;
    }
    argument[j][z] = '\0';
}
