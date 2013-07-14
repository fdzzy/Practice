#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

//#define DEBUG
#define BUF_SIZE 64

char* cards[] = {"3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A", "2", "Small Joker", "Big Joker"};
// Spade, Heart, Club, Diamond
char* suit[] = {"S", "H", "C", "D"};

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

void swap(int* val1, int* val2) {
    int temp = *val1;
    *val1 = *val2;
    *val2 = temp;
}

int main(int argc, char* argv[]) {

    srand(time(NULL));
    int player_cards[54];
    
    for(int i=0; i<54 ; ++i)
        player_cards[i] = i;
    
    for(int i=0; i<54 ; ++i) {
        int num = rand() % (54-i);
        swap(&player_cards[54-1-i], &player_cards[num]);
    }

#ifdef DEBUG
    for(int i=0; i<54 ; ++i) {
        printf("%d ", player_cards[i]);
    }
    printf("\n\n");
#endif
    
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
}
