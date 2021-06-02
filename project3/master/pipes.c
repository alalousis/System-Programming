#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "structs.h"
#include "functions.h"

void sendServerIPnPORT(int numWorkers, char *message, int bufferSize, char *serverIP, char *serverPort, int **fd){
    int i, messageSize;

    message[0] = '\0';
    strncat(message, serverIP, strlen(serverIP));
    strncat(message, "#", 1);
    strncat(message, serverPort, strlen(serverPort));
    strncat(message, "$", 1);
    messageSize = strlen(message);
    message[messageSize] = '\0';

    for(i=0; i<numWorkers; i++){
        if(write(fd[i][1], message, messageSize) != messageSize){
            perror("error in writing message");
        }
    }

}

void receiveServerIPnPORT(char *message, int bufferSize, char *serverIP, int *serverPort, int fdRead){
    int i, z, messageSize;
    char ip[32], port[8];

    message[0] = '\0';
    if( (messageSize=read(fdRead, message, bufferSize)) <= 0 ){
        perror("error in reading message");
    }
    message[messageSize] = '\0';

    i=0;
    z=0;
    while(1){
        while(message[z] != '#'){
            ip[i] = message[z];
            i++;
            z++;
        }
        ip[i] = '\0';

        z++;
        i=0;
        while(message[z] != '$'){
            port[i] = message[z];
            i++;
            z++;
        }
        port[i] = '\0';
        break;
    }

    strcpy(serverIP, ip);
    (*serverPort) = atoi(port);
}

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
