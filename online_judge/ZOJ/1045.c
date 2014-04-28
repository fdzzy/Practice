#include <stdio.h>

#define MAX 300

double val[MAX];

int main()
{
    int i;
    double num;

    val[0] = 0.5;

    for(i=1 ; i<MAX; i++){
        val[i] = val[i-1] + 1.0/(2+i);
    }

    while(scanf("%lf", &num) != EOF && num != 0) {
        for(i=0; i<MAX ; i++) {
            if(val[i] < num)
                continue;
            else {
                printf("%d card(s)\n", (i+1));
                break;
            }
        }
    }

    return 0;
}
