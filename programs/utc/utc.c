#include <stdio.h>
#include <string.h>

#define MIN_IN_SEC (60)
#define HOUR_IN_SEC (60 * MIN_IN_SEC)
#define DAY_IN_SEC (24 * HOUR_IN_SEC)
#define FOUR_YEAR_IN_SEC ((365*4+1) * DAY_IN_SEC)

#define BUF_SIZE 256
typedef unsigned int uint;
 
// 1970, 1971, 1973 is 365 days, while 1972 is 366 days
uint year_remainder[] = { 365*DAY_IN_SEC, 365*DAY_IN_SEC, 366*DAY_IN_SEC, 365*DAY_IN_SEC};
uint month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char* month_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
// 1970-1-1 is Thursday
char* weekday_name[] = {"Thu", "Fri", "Sat", "Sun", "Mon", "Tue", "Wed"};

/*
 * ts: timestamp
 * tz: timezone
 */
char* convertTimestamp(uint ts, int tz)
{
    uint year, mon, day, hour, min, sec, weekday;
    int i;
    static char buf[BUF_SIZE];
    char tz_str[BUF_SIZE] = {0}; 

    ts += tz * HOUR_IN_SEC;
    if(tz > 0) {
        snprintf(tz_str, BUF_SIZE, "+%d", tz);
    } else if(tz < 0){
        snprintf(tz_str, BUF_SIZE, "%d", tz);
    } 

    //calculate weekday
    weekday = (ts / DAY_IN_SEC) % 7; 

    //calculate year
    year = 1970 + ts / FOUR_YEAR_IN_SEC * 4;
    ts %= FOUR_YEAR_IN_SEC;
    for(i=0; i<4 ; ++i) {
        if(ts >= year_remainder[i]) 
            ts -= year_remainder[i]; 
        else
            break;
    }
    year += i;

    //check if it is the leap year
    if(i==2) ++month[1];

    //calculate month
    for(i=0; i<12 ; ++i) {
        uint month_in_sec = month[i] * DAY_IN_SEC;
        if(ts >= month_in_sec) 
            ts -= month_in_sec;
        else
            break;
    }
    mon = i;

    //calculate day
    day = 1 + ts / DAY_IN_SEC;
    ts %= DAY_IN_SEC;
    
    //calculate hour
    hour = ts / HOUR_IN_SEC;    
    ts %= HOUR_IN_SEC;

    //calculate min & sec
    min = ts / MIN_IN_SEC;
    sec = ts % MIN_IN_SEC;    

    snprintf(buf, BUF_SIZE, "%s-%02d-%04d %s %02d:%02d:%02d UTC%s\n", month_name[mon], day, year, weekday_name[weekday], hour, min, sec, tz_str); 

    return buf;
}

