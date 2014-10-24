#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
//#include <algorithm>

//#define DEBUG
#define BUF_SIZE 64

char* players[] = {"Joey", "Wei", "Lee", "Range", "Benny", "Zhitao", "Warren", "Frank", "Eric", "Vincent"};
char* fixed[] = {"Tom", "Dylan", "Patrick", "Yifan", "Cloud"};
char team[] = {'A', 'A', 'B', 'B', 'C', 'C', 'D', 'D', 'E', 'E'};

#if 0
char* getCardStr(int num) {
    static char buffer[BUF_SIZE]; 

#ifdef DEBUG
    sprintf(buffer, "%d", num);
#else    
    if(num > 51) {
        if(num == 52) {
            strncpy(buffer, cards[13], strlen(cards[13]));
            buffer[strlen(cards[13])] = '\0';
        }
        else {
            strncpy(buffer, cards[14], strlen(cards[14]));
            buffer[strlen(cards[14])] = '\0';
        }
    } else {
        int i = num / 4;
        int j = num % 4;
        strncpy(buffer, suit[j], strlen(suit[j]));
        buffer[strlen(suit[j])] = '\0';
        strcat(buffer, " ");
        strcat(buffer, cards[i]);
    }
#endif

    return buffer;
}
#endif

void swap(char** val1, char** val2) {
    char* temp = *val1;
    *val1 = *val2;
    *val2 = temp;
}

int main(int argc, char* argv[]) {

    int i;
    srand(time(NULL));
    char *temp[10];

    int length = sizeof(team)/sizeof(char);

    for(i=0; i<length; ++i) {
        temp[i] = players[i]; 
    }
    
    for(i=0; i<length ; ++i) {
        int num = rand() % (length-i);
        swap(&temp[length-1-i], &temp[num]);
    }

//#ifdef DEBUG
    for(i=0; i<length/2 ; ++i) {
        printf("Team %c:\n", 'A' + i);
        printf("%s, %s, %s\n\n", fixed[i], temp[i*2], temp[i*2+1]);
        //printf("%s: %c\n", players[i], team[i]);
    }
    //printf("\n\n");
//#endif
    
#if 0
    for(int j=0; j<3 ; ++j) {
        printf("Cards for player%d:\n", j+1);
        std::sort(player_cards+j*17, player_cards+j*17+17);
        for(int i=0; i<17 ; ++i) { 
            if(i)
                printf(", ");
            printf("%s", getCardStr(player_cards[j*17+i]));
        }
        printf("\n\n");
    }
    
    printf("The last three cards:\n");
    for(int i=51; i<54 ; ++i) {
        if(i!=51)
            printf(", ");
        printf("%s", getCardStr(player_cards[i]));
    }
    printf("\n");
#endif
}
