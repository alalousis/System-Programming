#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"
#include "functions.h"

void getdate(struct date *date, char* stringDate){
    char day[3], month[3], year[5];
    int i;

    for(i=0; i<2; i++){
        day[i] = stringDate[i];
        month[i] = stringDate[i+3];
    }
    day[2] = '\0';
    month[2] = '\0';

    for(i=0; i<4; i++){
        year[i] = stringDate[i+6];
    }
    year[4] = '\0';

    date->day = atoi(day);
    date->month = atoi(month);
    date->year = atoi(year);
}

int datecmp(struct date date1, struct date date2){            //if date1 > date2 returns -1;
    if(date1.year > date2.year){                              //if date1 = date2 returns 0;
        return -1;                                              //if date1 < date2 returns 1;
    }
    else if(date1.year == date2.year){
        if(date1.month > date2.month){
            return -1;
        }
        else if(date1.month == date2.month){
            if(date1.day > date2.day){
                return -1;
            }
            else if(date1.day == date2.day){
                return 0;
            }
        }
    }

    return 1;
}

int checkDate(char *d1, char *d2, char *message, int fdWrite){
    struct date date1, date2;
    int messageSize;

    getdate(&date1, d1);
    getdate(&date2, d2);
    
    if(strcmp(d1, "") != 0){
        if(strcmp(d2, "") == 0){
            printf("Date2 not inserted\n");
            strcpy(message, "!");
            messageSize = strlen(message);
            message[messageSize] = '\0';
            if(write(fdWrite, message, messageSize) != messageSize){
                perror("error in writing message");
            }
            return 0;
        }
    }

    if(datecmp(date1, date2) == -1){
        printf("Date2 is before date1\n");
        strcpy(message, "!");
        messageSize = strlen(message);
        message[messageSize] = '\0';
        if(write(fdWrite, message, messageSize) != messageSize){
            perror("error in writing message");
        }
        return 0;
    }
    return 1;
}
