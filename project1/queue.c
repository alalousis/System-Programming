#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"

void init(struct queueNode **queue){
    (*queue) = malloc(sizeof(struct queueNode));
    (*queue)->heapNode = NULL;
    (*queue)->nextQueueNode = NULL;
}

void push(struct queueNode **queue, struct heapNode *heapNode){
    struct queueNode *currentQueueNode;

    currentQueueNode = (*queue);
    if(isEmpty((*queue))){
        (*queue)->heapNode = heapNode;
        (*queue)->nextQueueNode = NULL;
    }
    else{
        while(currentQueueNode->nextQueueNode != NULL){
            currentQueueNode = currentQueueNode->nextQueueNode;
        }
        currentQueueNode->nextQueueNode = malloc(sizeof(struct queueNode));
        currentQueueNode->nextQueueNode->heapNode = heapNode;
        currentQueueNode->nextQueueNode->nextQueueNode = NULL;
    }
}

struct heapNode * pop(struct queueNode **queue){
    struct queueNode *tempQueueNode;
    struct heapNode *tempHeapNode;

    tempQueueNode = (*queue);
    (*queue) = (*queue)->nextQueueNode;
    tempHeapNode = tempQueueNode->heapNode;
    free(tempQueueNode);
    return tempHeapNode;
}

int isEmpty(struct queueNode *queue){
    if(queue->heapNode == NULL){
        return 1;
    }
    return 0;
}
