#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "structs.h"
#include "functions.h"
#include "variables.h"

int receiveStatistics(int newsock){
    int j, z, messageSize, workerPortNum;
    char message[64], workerPort[8], numWorkersStr[4];

    if((messageSize = read(newsock, message, 10)) < 0){
        perror("Error in read");
    }
    message[messageSize] = '\0';

    j=0;
    z=0;
    while(1){
        while(message[z] != '#'){
            workerPort[j] = message[z];
            j++;
            z++;
        }
        workerPort[j] = '\0';

        z++;
        j=0;
        while(message[z] != '$'){
            numWorkersStr[j] = message[z];
            j++;
            z++;
        }
        numWorkersStr[j] = '\0';
        break;
    }

    workerPortNum = atoi(workerPort);

    return workerPortNum;
}

int sendQuery(char *workerIP, int *workerPorts, char *query, int numWorkers, int *sockets){
    int i;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    for(i=0; i<numWorkers; i++){
        if ((sockets[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0){          //send query to worker
            perror("Error in socket");
        }

        if((rem = gethostbyname(workerIP)) == NULL){
            herror("Error in gethostbyname");
        }

        server.sin_family = AF_INET;
        memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
        server.sin_port = htons(workerPorts[i]);
        if(connect(sockets[i], serverptr, sizeof(server)) < 0){
            perror("Error in connect");
        }

        if( write(sockets[i], query, strlen(query)) < 0 ){
            perror("Error in write");
        }
    }
}
