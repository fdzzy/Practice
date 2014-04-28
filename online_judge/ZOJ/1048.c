#include <stdio.h>

#define MON 12

int main()
{
    double balance, income = 0, mean;
    int i;
    for(i=0 ; i<MON ; i++) {
        scanf("%lf", &balance);
        income += balance;
    }

    mean = income / MON;

    printf("$%.2f\n", mean); 
    
    return 0;
}
