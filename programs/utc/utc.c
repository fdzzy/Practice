#include <stdio.h>
#include <stdlib.h>

#define MIN_IN_SEC (60)
#define HOUR_IN_SEC (60 * MIN_IN_SEC)
#define DAY_IN_SEC (24 * HOUR_IN_SEC)
#define FOUR_YEAR_IN_SEC ((365*4+1) * DAY_IN_SEC)
 
// 1970, 1971, 1973 is 365 days, while 1972 is 366 days
int year_remainder[] = { 365*DAY_IN_SEC, 365*DAY_IN_SEC, 366*DAY_IN_SEC, 365*DAY_IN_SEC};
int month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char* month_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
// 1970-1-1 is Thursday
char* weekday_name[] = {"Thu", "Fri", "Sat", "Sun", "Mon", "Tue", "Wed"};

int main(int argc, char** argv)
{
    if(argc != 2) {
        printf("Usage: utc seconds\n");
        exit(-1);
    }

    int utc = atoi(argv[1]);
    //printf("%d\n", utc);
    
    int year, mon, day, hour, min, sec, weekday;
    int i;

    //calculate weekday
    weekday = (utc / DAY_IN_SEC) % 7; 

    //calculate year
    year = 1970 + utc / FOUR_YEAR_IN_SEC * 4;
    utc %= FOUR_YEAR_IN_SEC;
    for(i=0; i<4 ; ++i) {
        if(utc >= year_remainder[i]) 
            utc -= year_remainder[i]; 
        else
            break;
    }
    year += i;

    //check if it is the leap year
    if(i==2) ++month[1];

    //calculate month
    for(i=0; i<12 ; ++i) {
        int month_in_sec = month[i] * DAY_IN_SEC;
        if(utc >= month_in_sec) 
            utc -= month_in_sec;
        else
            break;
    }
    mon = i;

    //calculate day
    day = 1 + utc / DAY_IN_SEC;
    utc %= DAY_IN_SEC;
    
    //calculate hour
    hour = utc / HOUR_IN_SEC;    
    utc %= HOUR_IN_SEC;

    //calculate min & sec
    min = utc / MIN_IN_SEC;
    sec = utc % MIN_IN_SEC;    

    printf("%s-%02d-%04d %s %02d:%02d:%02d UTC\n", month_name[mon], day, year, weekday_name[weekday], hour, min, sec); 

    return 0;
}
