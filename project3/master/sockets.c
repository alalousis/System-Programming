#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "structs.h"
#include "functions.h"

void sendStatistics(int workerPortNum, int serverPort, char *serverIP, char *numWorkers){
    int sock;
    char statistics[8];
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){          //send statistics
        perror("Error in socket");
    }

    if((rem = gethostbyname(serverIP)) == NULL){
        herror("Error in gethostbyname");
    }

    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(serverPort);
    if(connect(sock, serverptr, sizeof(server)) < 0){
        perror("Error in connect");
    }

    sprintf(statistics, "%d", workerPortNum);
    strncat(statistics, "#", 1);
    strncat(statistics, numWorkers, strlen(numWorkers));
    strncat(statistics, "$", 1);
    statistics[strlen(statistics)] = '\0';

    if( write(sock, statistics, strlen(statistics)) < 0 ){
        perror("Error in write");
    }

    if( write(sock, statistics, strlen(statistics)) < 0 ){
        perror("Error in write");
    }
}
