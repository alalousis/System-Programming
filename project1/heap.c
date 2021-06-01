#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

void insertHeap(struct heapNode **root, int value, char *key){
    struct heapNode *currentHeapNode;
    struct heapNode *heap;
    struct queueNode *queue;

    init(&queue);

    if((*root) == NULL){
        (*root) = malloc(sizeof(struct heapNode));
        (*root)->value = value;
        strcpy((*root)->key, key);
        (*root)->leftChild = malloc(sizeof(struct heapNode));
        (*root)->leftChild->value = -1;
        (*root)->rightChild = malloc(sizeof(struct heapNode));
        (*root)->rightChild->value = -1;
        return;
    }

    currentHeapNode = findHeapNode((*root), &queue);
    currentHeapNode->value = value;
    strcpy(currentHeapNode->key, key);
    currentHeapNode->leftChild = malloc(sizeof(struct heapNode));
    currentHeapNode->leftChild->value = -1;
    currentHeapNode->rightChild = malloc(sizeof(struct heapNode));
    currentHeapNode->rightChild->value = -1;

    free(queue);
}

void heapify(struct heapNode **root, int *changes){
    struct heapNode *maxChild;

    if((*root)->value == -1 || (*root) == NULL){
        return;
    }

    maxChild = max((*root)->leftChild, (*root)->rightChild);
    if((*root)->value < maxChild->value){
        (*changes)++;
        swap(root, &maxChild);
        heapify(&maxChild, changes);
    }

    heapify(&((*root)->leftChild), changes);
    heapify(&((*root)->rightChild), changes);
}

struct heapNode * findHeapNode(struct heapNode *root, struct queueNode **queue){
    struct heapNode *currentHeapNode;

    push(queue, root);
    push(queue, root->leftChild);
    push(queue, root->rightChild);

    currentHeapNode = pop(queue);
    currentHeapNode = pop(queue);
    while(currentHeapNode->value != -1){
        push(queue, currentHeapNode->leftChild);
        push(queue, currentHeapNode->rightChild);
        currentHeapNode = pop(queue);
    }

    return currentHeapNode;
}

struct heapNode * max(struct heapNode *heapNode1, struct heapNode *heapNode2){
    if(heapNode1->value >= heapNode2->value){
        return heapNode1;
    }
    return heapNode2;
}

void swap(struct heapNode **heapNode1, struct heapNode **heapNode2){
    int tempvalue;
    char tempkey[16];

    tempvalue = (*heapNode1)->value;
    strcpy(tempkey, (*heapNode1)->key);

    (*heapNode1)->value = (*heapNode2)->value;
    strcpy((*heapNode1)->key, (*heapNode2)->key);

    (*heapNode2)->value = tempvalue;
    strcpy((*heapNode2)->key, tempkey);
}

struct heapNode * top(struct heapNode **root){
    struct heapNode *tempHeapNode, *currentHeapNode;
    struct queueNode *queue;

    tempHeapNode = malloc(sizeof(struct heapNode));
    tempHeapNode->value = (*root)->value;
    strcpy(tempHeapNode->key, (*root)->key);

    init(&queue);

    push(&queue, (*root)->leftChild);
    if((*root)->leftChild->value != -1){
        push(&queue, (*root)->leftChild);
    }
    if((*root)->rightChild->value != -1){
        push(&queue, (*root)->rightChild);
    }

    if(queue != NULL){
        currentHeapNode = pop(&queue);
    }
    if(queue != NULL){
        currentHeapNode = pop(&queue);
    }

    while(queue != NULL){
        if(currentHeapNode->leftChild->value != -1){
            push(&queue, currentHeapNode->leftChild);
        }
        if(currentHeapNode->rightChild->value != -1){
            push(&queue, currentHeapNode->rightChild);
        }
        currentHeapNode = pop(&queue);
    }

    (*root)->value = currentHeapNode->value;
    strcpy((*root)->key, currentHeapNode->key);
    currentHeapNode->value = -1;
    strcpy(currentHeapNode->key, "");

    free(queue);
    return tempHeapNode;
}
