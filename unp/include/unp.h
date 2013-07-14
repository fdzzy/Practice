/*
Unix Networking Programming Header File
*/

#ifndef      __UNP_H
#define     __UNP_H 

/* Basic Headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Network Headers */
#include <sys/socket.h>     /* basic socket definitions */
#include <netinet/in.h>     /* sockaddr_in() and other Internet defns */

#define SA struct sockaddr

#define MAXLINE 512
#define LISTENQ 30

/* Error handling */
#include <errno.h>
#define VSNPRINTF
extern int daemon_proc;
void err_sys(const char* fmt, ...);
void err_quit(const char* fmt, ...);
#endif
