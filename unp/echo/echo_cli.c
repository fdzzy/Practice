#include "unp.h"

int main(int argc, char** argv)
{
    int         sockfd, n;
    int         counter = 0;
    char        recvline[MAXLINE + 1];
    char        buf[MAXLINE];
    struct sockaddr_in  servaddr;

    if(argc != 2)
        err_quit("usage: daytime_cli <IPaddress>");

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
        err_sys("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);  /* day time server */
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0 )
        err_quit("inet_pton error for %s", argv[1]);

    if( connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0 )
        err_sys("connect error");

    fgets(buf, MAXLINE, stdin);
    buf[strlen(buf)-1] = '\0';    
    printf("Sending: \"%s\"\n", buf);

    n = write(sockfd, buf, strlen(buf));

    while( (n = read(sockfd, recvline, MAXLINE)) > 0 ) {
        counter++;
        recvline[n] = 0;    /* null terminated */
        printf("Receiving: \"%s\"\n", recvline);
    }
    //printf("Read counter: %d\n", counter);

    if( n < 0 )
        err_sys("read error");

    exit(0);
}
