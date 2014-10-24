#include "unp.h"
#include <time.h>

#define PORT 5555

int main(int argc, char* argv)
{
    int     listenfd, connfd, i;
    struct  sockaddr_in servaddr;
    char    buf[MAXLINE];
    char    buf2[MAXLINE];
    size_t  size;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        err_quit("socket error");
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);  

    if( (bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0 )
        err_sys("bind error");

    if( (listen(listenfd, LISTENQ)) < 0 )
        err_sys("listen error");

    printf("DES OTP server starts listening\n");
     
    while(1) {
        if( (connfd = accept(listenfd, (SA *) NULL, NULL)) < 0 )
            err_sys("accept error");

        size = read(connfd, buf, MAXLINE); 
        buf[size] = '\0';
        printf("received client string: %s\n", buf);

        if(strncmp(buf, "GET DES OTP", MAXLINE) == 0) {
            printf("Please input DES OTP: ");
            fgets(buf2, MAXLINE, stdin); 
            //printf("buf2: %s\n", buf2);

            if(write(connfd, buf2, strlen(buf2)) < 0)
                err_sys("write error");
        }

        close(connfd);
    }
}
