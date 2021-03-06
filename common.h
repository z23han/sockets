#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SERV_PORT   12345
#define MAXLINE 4096
#define LISTENQ  1024


struct acceptor 
{
    int listen_port; 
    int listen_fd;
};


#endif  // COMMON_H
