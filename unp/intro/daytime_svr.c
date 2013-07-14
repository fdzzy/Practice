#include "unp.h"
#include <time.h>

int main(int argc, char* argv)
{
    int     listenfd, connfd;
    struct  sockaddr_in servaddr;
    char    buf[MAXLINE];
    time_t  ticks;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        err_quit("socket error");
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);  /* daytime server */

    if( (bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0 )
        err_sys("bind error");

    if( (listen(listenfd, LISTENQ)) < 0 )
        err_sys("listen error");
     
    while(1) {
        if( (connfd = accept(listenfd, (SA *) NULL, NULL)) < 0 )
            err_sys("accept error");
        
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        int i=0;
        for(i=0; i<50 ; i++){
        if(write(connfd, buf, strlen(buf)) < 0)
            err_sys("write error");
        }

        close(connfd);
    }
}
