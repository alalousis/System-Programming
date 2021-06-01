#include <stdio.h>
#include <stdlib.h>
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
