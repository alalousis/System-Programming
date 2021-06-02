#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "functions.h"
#include "structs.h"
#include "variables.h"

extern int queriesRead;

void terror(char *s, int error){
    fprintf(stderr, "%s: %s\n", s, strerror(error));
}

void *threadRoutine(void *arguments){
    int i, j, numQueries, numThreads, error, sock, servPort;
    char **queries, query[100], servIP[20];
    struct arguments *args;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    args = (struct arguments *)arguments;
    numQueries = args->numQueries;
    queries = args->queries;
    numThreads = args->numThreads;
    servPort = args->servPort;
    sock = args->sock;
    strcpy(servIP, args->servIP);

    if(error = pthread_mutex_lock(&read_mutex)){
        terror("pthread_mutex_lock", error);
    }
    queriesRead++;
    query[0] = '\0';
    strcpy(query, queries[queriesRead-1]);

    pthread_cond_broadcast(&read_cond);
    if(error = pthread_mutex_unlock(&read_mutex)){
        terror("pthread_mutex_unlock", error);
    }

    if(error = pthread_mutex_lock(&read_mutex)){
        terror("pthread_mutex_lock", error);
    }
    while(queriesRead < numThreads){
        pthread_cond_wait(&read_cond, &read_mutex);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){          //send query
        perror("Error in socket");
    }

    if((rem = gethostbyname(servIP)) == NULL){
        herror("Error in gethostbyname");
    }

    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(servPort);
    if(connect(sock, serverptr, sizeof(server)) < 0){
        perror("Error in connect");
    }

    if( write(sock, query, strlen(query)) < 0 ){
        perror("Error in write");
    }

    pthread_cond_broadcast(&read_cond);
    if(error = pthread_mutex_unlock(&read_mutex)){
        terror("pthread_mutex_unlock", error);
    }

    sleep(3);

    while(queriesRead < numQueries){
        if(error = pthread_mutex_lock(&read_mutex)){
            terror("pthread_mutex_lock", error);
        }
        queriesRead++;
        query[0] = '\0';
        strcpy(query, queries[queriesRead-1]);

        pthread_cond_broadcast(&read_cond);

        if(error = pthread_mutex_unlock(&read_mutex)){
            terror("pthread_mutex_unlock", error);
        }

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){          //send query
            perror("Error in socket");
        }

        if((rem = gethostbyname(servIP)) == NULL){
            herror("Error in gethostbyname");
        }

        server.sin_family = AF_INET;
        memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
        server.sin_port = htons(servPort);
        if(connect(sock, serverptr, sizeof(server)) < 0){
            perror("Error in connect");
        }

        if( write(sock, query, strlen(query)) < 0 ){
            perror("Error in write");
        }

        if(error = pthread_mutex_unlock(&read_mutex)){
            terror("pthread_mutex_unlock", error);
        }

        sleep(3);
    }
}
