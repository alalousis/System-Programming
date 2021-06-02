#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "structs.h"
#include "functions.h"
#include "variables.h"

extern int queriesToReceive;

void init(struct circularBuffer *cirBuff, int bufferSize){
    cirBuff->fds = malloc(bufferSize*sizeof(int));
    cirBuff->start = 0;
    cirBuff->end = -1;
    cirBuff->count = 0;
}

void place(struct circularBuffer *cirBuff, int data, int bufferSize){
    pthread_mutex_lock (& mtx);
    while (cirBuff->count >= bufferSize ) {   //Found Buffer Full
        pthread_cond_wait (&cond_nonfull, &mtx);
    }
    cirBuff->end = (cirBuff->end + 1) % bufferSize ;
    cirBuff->fds[cirBuff->end] = data ;
    cirBuff->count++;
    pthread_mutex_unlock (&mtx);
}

int obtain(struct circularBuffer *cirBuff, int bufferSize){
    int data = 0;

    pthread_mutex_lock (& mtx);
    while (cirBuff->count <= 0) {    //Found Buffer Empty
        pthread_cond_wait (&cond_nonempty , &mtx);
    }
    data = cirBuff->fds[cirBuff->start];
    cirBuff->start = (cirBuff->start+1) % bufferSize ;
    cirBuff->count--;
    pthread_mutex_unlock(&mtx);
    return data;
}
