#include <stdio.h>
#include <stdlib.h>

char* convertTimestamp(uint ts, int tz);

int main(int argc, char** argv)
{
    char *str;

    if(argc < 2) {
        printf("\nUsage: utc seconds [timezone]\n");
        printf("Example:\n");
        printf("       utc 1385625240\n");
        printf("       utc 1385625240 +8\n");
        printf("\n");
        exit(-1);
    }

    int utc = atoi(argv[1]);
    //printf("%d\n", utc);

    int tz = 0;
    if(argc >= 3) {
        tz = atoi(argv[2]);
        //printf("%d\n", tz);
    }

    str = convertTimestamp(utc, tz); 
    printf("%s", str);
    
    return 0;
}
