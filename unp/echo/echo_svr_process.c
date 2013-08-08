#include "unp.h"
#include <time.h>

void process_connection(int connfd)
{
    char    buf[MAXLINE];
    char    buf2[MAXLINE];
    size_t  size;

    printf("Child process %d processing connection\n", getpid());
    size = read(connfd, buf, MAXLINE); 
    buf[size] = '\0';
    printf("received client string: %s\n", buf);

    snprintf(buf2, MAXLINE, "Server responding: %s", buf);
     
    if(write(connfd, buf2, strlen(buf2)) < 0)
        err_sys("write error");

    close(connfd);
} 

int main(int argc, char* argv)
{
    int     listenfd, connfd, i, pid;
    struct  sockaddr_in servaddr;

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

    printf("ECHO server starts listening\n");
     
    while(1) {
        if( (connfd = accept(listenfd, (SA *) NULL, NULL)) < 0 )
            err_sys("accept error");

        pid = fork();
        if(pid < 0) {
            err_sys("fork error");
        } else if(pid == 0) {
            /* process connection in child process */
            process_connection(connfd);
            exit(0);
        }
        
        close(connfd);
    }
}
