#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"

void freeRecordsHashTable(struct listNode **recordsHashtable, int recordsHashtableSize){
    int i;
    struct listNode *currentListNode, *tempListNode;

    for(i=0; i<recordsHashtableSize; i++){
        currentListNode = recordsHashtable[i];
        while(currentListNode != NULL){
            free(currentListNode->record->recordID);
            free(currentListNode->record->patientFirstName);
            free(currentListNode->record->patientLastName);
            free(currentListNode->record->diseaseID);
            free(currentListNode->record->country);
            free(currentListNode->record->entryDate);
            free(currentListNode->record->exitDate);
            free(currentListNode->record);
            tempListNode = currentListNode;
            currentListNode = currentListNode->nextRecord;
            free(tempListNode);
		}

    }
    free(recordsHashtable);
}

void freeHashTable(struct bucket **Hashtable, int HashtableSize, int entriesPerBucket){
    int i, j;
    struct bucket *currentBucket, *tempBucket;

    for(i=0; i<HashtableSize; i++){
        currentBucket = Hashtable[i];
        while(currentBucket != NULL){
            for(j=0; j<entriesPerBucket; j++){
                freeBinaryTree(currentBucket->values[j]);
            }
            tempBucket = currentBucket;
			currentBucket = currentBucket->nextBucket;
            free(tempBucket);
		}
    }
    free(Hashtable);
}

void freeBinaryTree(struct treeNode *root){
    struct listNode *currentListNode, *tempListNode;

    if(root == NULL){
        return;
    }

    freeBinaryTree(root->leftChild);
    freeBinaryTree(root->rightChild);

    currentListNode = root->recordsList;
    while(currentListNode != NULL){
        tempListNode = currentListNode;
        currentListNode = currentListNode->nextRecord;
        free(tempListNode);
    }
    free(root);
}

void freeHeap(struct heapNode *root){
    if(root == NULL){
        return;
    }

    freeHeap(root->leftChild);
    freeHeap(root->rightChild);

    free(root);
}
