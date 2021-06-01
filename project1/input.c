#include <stdio.h>
#include "structs.h"
#include "functions.h"

int saveRecords(FILE *fd, struct listNode **recordsHashtable, int recordsHashtableSize){
    char ch, newRecord[7][20];
    int i, j;

    ch = fgetc(fd);
    while(ch != EOF){
        i=0;
        while(ch != '\n' ){
            j=0;
            while (ch != ' ' && ch != '\t' && ch != '\n' ){
                newRecord[i][j] = ch;
                j++;
                ch = fgetc(fd);
            }
            newRecord[i][j] = '\0';

            if(ch != '\n' ){
                ch = fgetc(fd);
            }
            i++;
        }

        if(checkRecordID(recordsHashtable, recordsHashtableSize, newRecord[0]) == -1){
            return -1;
        }

        recordsHash(recordsHashtable, recordsHashtableSize, newRecord);
        ch = fgetc(fd);
    }

    return 0;
}
