#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

void insertTree(struct treeNode **root, struct record *patient, struct date date1){
    struct treeNode *currentTreeNode;
    struct listNode *currentListNode;

    if((*root) == NULL){
        (*root) = malloc(sizeof(struct treeNode));
        (*root)->recordsList = malloc(sizeof(struct listNode));
        (*root)->recordsList->nextRecord = NULL;
        (*root)->recordsList->record = malloc(sizeof(struct record));
        (*root)->recordsList->record = patient;
        (*root)->date = date1;
        (*root)->leftChild = NULL;
        (*root)->rightChild = NULL;
        (*root)->height = 1;
        return;
    }

    currentTreeNode = (*root);
    while(1){
        if (datecmp(date1, currentTreeNode->date) == 0){                        //If date1 exists
            currentListNode = currentTreeNode->recordsList;
            if(strcmp(currentListNode->record->recordID, patient->recordID) == 0){
                return;
            }
            while(currentListNode->nextRecord != NULL){
                if(strcmp(currentListNode->record->recordID, patient->recordID) == 0){
                    return;
                }
                currentListNode = currentListNode->nextRecord;
            }
            currentListNode->nextRecord = malloc(sizeof(struct listNode));
            currentListNode->nextRecord->record = patient;
            currentListNode->nextRecord->nextRecord = NULL;
            return;
        }
        else if(datecmp(date1, currentTreeNode->date) == 1){                    //If date1 < currentTreeNode->date
            if(currentTreeNode->leftChild == NULL){
                currentTreeNode->leftChild = malloc(sizeof(struct treeNode));
                currentTreeNode->leftChild->recordsList = malloc(sizeof(struct listNode));
                currentTreeNode->leftChild->recordsList->nextRecord = NULL;
                currentTreeNode->leftChild->recordsList->record = malloc(sizeof(struct record));
                currentTreeNode->leftChild->recordsList->record = patient;
                currentTreeNode->leftChild->date = date1;
                currentTreeNode->leftChild->leftChild = NULL;
                currentTreeNode->leftChild->rightChild = NULL;
                currentTreeNode->leftChild->height = 1;
                break;
            }
            else{
                currentTreeNode = currentTreeNode->leftChild;
            }
        }
        else if(datecmp(date1, currentTreeNode->date) == -1){                   //If date1 > currentTreeNode->date
            if(currentTreeNode->rightChild == NULL){
                currentTreeNode->rightChild = malloc(sizeof(struct treeNode));
                currentTreeNode->rightChild->recordsList = malloc(sizeof(struct listNode));
                currentTreeNode->rightChild->recordsList->nextRecord = NULL;
                currentTreeNode->rightChild->recordsList->record = malloc(sizeof(struct record));
                currentTreeNode->rightChild->recordsList->record = patient;
                currentTreeNode->rightChild->date = date1;
                currentTreeNode->rightChild->leftChild = NULL;
                currentTreeNode->rightChild->rightChild = NULL;
                currentTreeNode->rightChild->height = 1;
                break;
            }
            else{
                currentTreeNode = currentTreeNode->rightChild;
            }
        }
    }

    balance(root);
    return;
}

void balance(struct treeNode **root){
    if((*root) == NULL){
        return;
    }

    if((*root)->leftChild != NULL){
        balance(&((*root)->leftChild));
    }

    if((*root)->rightChild != NULL){
        balance(&((*root)->rightChild));
    }

    if((*root)->leftChild != NULL && (*root)->rightChild != NULL){
        (*root)->height = maxHeight((*root)->leftChild->height, (*root)->rightChild->height) + 1;

        if((*root)->leftChild->height - (*root)->rightChild->height == 2){
            if((*root)->leftChild->leftChild != NULL){
                (*root) = rotateR(root);
            }
            else{
                (*root) = rotateLR(root);
            }
        }
        else if((*root)->leftChild->height - (*root)->rightChild->height == -2){
            if((*root)->rightChild->rightChild != NULL){
                (*root) = rotateL(root);
            }
            else{
                (*root) = rotateRL(root);
            }
        }
    }
    else if((*root)->leftChild != NULL && (*root)->rightChild == NULL){

        (*root)->height = (*root)->leftChild->height + 1;

        if((*root)->leftChild->height == 2){
            if((*root)->leftChild->leftChild != NULL){
                (*root) = rotateR(root);
            }
            else{
                (*root) = rotateLR(root);
            }
        }
    }
    else if((*root)->leftChild == NULL && (*root)->rightChild != NULL){
        (*root)->height = (*root)->rightChild->height + 1;

        if(0 - (*root)->rightChild->height == -2){
            if((*root)->rightChild->rightChild != NULL){
                (*root) = rotateL(root);
            }
            else{
                (*root) = rotateRL(root);
            }
        }
    }
}

int maxHeight(int a, int b){
    if(a >= b){
        return a;
    }
    return b;
}

void getHeight(struct treeNode **root){
    if((*root)->leftChild != NULL && (*root)->rightChild != NULL){
        (*root)->height = maxHeight((*root)->leftChild->height, (*root)->rightChild->height) + 1;
    }
    else if((*root)->leftChild != NULL && (*root)->rightChild == NULL){
        (*root)->height = (*root)->leftChild->height + 1;
    }
    else if((*root)->leftChild == NULL && (*root)->rightChild != NULL){
        (*root)->height = (*root)->rightChild->height + 1;
    }
    else if((*root)->leftChild == NULL && (*root)->rightChild == NULL){
        (*root)->height = 1;
    }
}

struct treeNode * rotateR(struct treeNode **root){
    struct treeNode *tempTreeNode;

    tempTreeNode = (*root)->leftChild;
    (*root)->leftChild = tempTreeNode->rightChild;
    tempTreeNode->rightChild = (*root);

    getHeight(root);
    getHeight(&tempTreeNode);

    return tempTreeNode;
}

struct treeNode * rotateL(struct treeNode **root){
    struct treeNode *tempTreeNode;

    tempTreeNode = (*root)->rightChild;
    (*root)->rightChild = tempTreeNode->leftChild;
    tempTreeNode->leftChild = (*root);

    getHeight(root);
    getHeight(&tempTreeNode);

    return tempTreeNode;
}

struct treeNode * rotateRL(struct treeNode **root){
    (*root)->rightChild = rotateR(&((*root)->rightChild));
    return rotateL(root);
}

struct treeNode * rotateLR(struct treeNode **root){
    (*root)->leftChild = rotateL(&((*root)->leftChild));
    return rotateR(root);
}
