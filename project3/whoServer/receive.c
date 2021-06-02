#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "structs.h"
#include "functions.h"

int diseaseFrequencyReceive(int *sockets, int numWorkers){
    int i, messageSize, result, worker;
    char message[200];

    result = 0;
    for(i=0; i<numWorkers; i++){
        if( (messageSize=read(sockets[i], message, 200)) <= 0 ){
            perror("error in reading message");
        }

        if(message[0] == '!'){
            return 0;
        }

        if(message[0] == 'n'){          //if worket has been interrupted
            continue;
        }

        message[messageSize] = '\0';
        result += atoi(message);
    }
    printf("%d\n", result);
    return 1;
}

int topAgeRangesReceive(int *sockets, int numWorkers){
    int i, j, z, messageSize, worker;
    char perchentage[8], message[200];

    for(i=0; i<numWorkers; i++){
        if( (messageSize=read(sockets[i], message, 200)) <= 0 ){
            perror("error in reading message");
        }
        message[messageSize] = '\0';
        if(message[0] == '!'){
            return 0;
        }

        if(message[0] == 'n'){          //if country doesn't belong to worker
            continue;
        }

        j=0;
        z=1;
        while(1){
            if(message[z] == '#'){
                z++;
                if(message[z] == '$'){
                    break;
                }

                if(message[z] == '0'){
                    printf("0-20: ");
                }
                else if(message[z] == '2'){
                    printf("21-40: ");
                }
                else if(message[z] == '4'){
                    printf("41-60: ");
                }
                else if(message[z] == '6'){
                    printf("60+: ");
                }

                z++;
                j=0;
            }


            while(message[z] != '#'){
                perchentage[j] = message[z];
                j++;
                z++;
            }
            perchentage[j] = '\0';
            printf("%s%%\n", perchentage);
        }

    }

    return 1;
}

int searchByIDReceive(int *sockets, int numWorkers){
    int i, j, z, messageSize, worker;
    char word[16], message[200];

    for(i=0; i<numWorkers; i++){
            if( (messageSize=read(sockets[i], message, 200)) <= 0 ){
                perror("error in reading message");
            }
            message[messageSize] = '\0';
            //printf("%s\n", message);
            if(message[0] == 'n'){          //if worker has been interrupted
                continue;
            }

            if(message[0] == '$'){          //if id doesn't belong to worker
                continue;
            }

            j=0;
            for(z=0; z<messageSize; z++){
                if(message[z] == '#'){
                    word[j] = '\0';
                    printf("%s ", word);
                    j=0;
                    z++;
                }

                if(message[z] == '$'){
                    break;
                }
                word[j] = message[z];
                j++;
            }

            printf("\n");
            return 1;

    }
    printf("Record ID doesn't exist\n");
    return 1;
}

int numPatientAdmissionsReceive(int *sockets, int numWorkers){
    int i, j, z, k, messageSize, worker;
    char country[16], counter[4], message[200];

    for(i=0; i<numWorkers; i++){
        if( (messageSize=read(sockets[i], message, 200)) <= 0 ){
            perror("error in reading message");
        }
        message[messageSize] = '\0';
        //printf("%s\n", message);

        if(message[0] == '!'){
            return 0;
        }

        if(message[0] == 'n'){          //if worker has been interrupted
            continue;
        }

        if(message[0] == '$'){
            continue;
        }

        j=0;
        k=0;
        for(z=0; z<messageSize; z++){
            if(message[z] == '#'){
                country[j] = '\0';
                j=0;
                z++;

                while(message[z] != '/'){
                    counter[k] = message[z];
                    k++;
                    z++;
                }

                counter[k] = '\0';
                printf("%s %s\n", country, counter);
                k=0;
                z++;
            }

            if(message[z] == '$'){
                break;
            }

            country[j] = message[z];
            j++;
        }
    }
    return 1;
}

int numPatientDischargesReceive(int *sockets, int numWorkers){
    int i, j, z, k, messageSize, worker;
    char country[16], counter[4], message[200];

    for(i=0; i<numWorkers; i++){
        if( (messageSize=read(sockets[i], message, 200)) <= 0 ){
            perror("error in reading message");
        }
        message[messageSize] = '\0';
        //printf("%s\n", message);
        if(message[0] == '!'){
            return 0;
        }

        if(message[0] == '$'){
            continue;
        }

        j=0;
        k=0;
        for(z=0; z<messageSize; z++){
            if(message[z] == '#'){
                country[j] = '\0';
                j=0;
                z++;

                while(message[z] != '/'){
                    counter[k] = message[z];
                    k++;
                    z++;
                }

                counter[k] = '\0';
                printf("%s %s\n", country, counter);
                k=0;
                z++;
            }

            if(message[z] == '$'){
                break;
            }

            country[j] = message[z];
            j++;
        }
    }
    return 1;
}
