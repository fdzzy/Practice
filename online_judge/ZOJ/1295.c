#include <stdio.h>
#include <string.h>

#define MAX 70

int main()
{
    char buf[MAX];
    int line, i, j, len;

    scanf("%d\n", &line);
    
    if(line <= 0) {
        return -1;
    }

    for(i=0; i<line; i++) {
        fgets(buf, MAX, stdin);
        len = strlen(buf);
        //printf("i: %d, str: \"%s\", len: %d\n", i, buf, len);
        len--;
        for(j=0; j<(len/2) ; j++) {
            char temp = buf[j];
            buf[j] =  buf[len-1-j]; 
            buf[len-1-j] = temp;
        }
        printf("%s", buf);
    }
    
    return 0;
}
