#include "unp.h"

#define PORT 5555

int main(int argc, char** argv)
{
    int         sockfd, n, size;
    int         counter = 0;
    char        recvline[MAXLINE + 1];
    char        buf[MAXLINE];
    struct sockaddr_in  servaddr;
    FILE        *fp;

    if(argc != 2) {
        printf("usage: echo_cli <IPaddress>");
        exit(-1);
    }

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        printf("socket error");
        exit(-1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);  
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0 ) {
        printf("inet_pton error for %s", argv[1]);
        exit(-1);
    }

    if( connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0 ) {
        printf("connect error");
        exit(-1);
    }

    snprintf(buf, MAXLINE, "GET DES OTP");
    buf[strlen(buf)] = '\0';    
    //printf("Sending: \"%s\"\n", buf);

    n = write(sockfd, buf, strlen(buf));

    buf[0] = '\0';
    size = MAXLINE;
    while( (n = read(sockfd, recvline, MAXLINE)) > 0 ) {
        counter++;
        recvline[n] = 0;    /* null terminated */
        strncat(buf, recvline, size);
        size -= strlen(recvline);
    }
    //printf("Read counter: %d\n", counter);
    if(buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';
    printf("Receiving: \"%s\"\n", buf);

    fp = fopen("PASSWD", "w");
    if(fp == NULL) {
        printf("Can't open file 'PASSWD'");
        exit(-1);
    }
    fprintf(fp, "%s", buf);

    if( n < 0 ) {
        printf("read error");
        exit(-1);
    }

    exit(0);
}
