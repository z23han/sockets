#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>


#define BUFFER_SIZE 2048
#define MAXLINE 2048


/*
 * this example is based on tcp stream and local_path socket
 * */
int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: ipc_server <local_path>");
        exit(1);
    }

    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_un cliaddr, servaddr;

    // create a listenfd socket fd
    listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listenfd < 0) {
        fprintf(stderr, "socket create failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    // emphasize: we need the local_path to bind to sun_path
    char *local_path = argv[1];
    unlink(local_path);

    // fill up the servaddr struct info
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, local_path);

    // if bind fails, exit
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "bind failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    // if listen fails, exit
    if (listen(listenfd, 1024) < 0) {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }
    
    clilen = sizeof(cliaddr);

    // if we fail to accept from client, exit
    if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
        if (errno == EINTR) {
            fprintf(stderr, "accept failed\n");
            exit(1);
        } else {
            fprintf(stderr, "accept failed\n");
            exit(1);
        }
    }

    char buf[BUFFER_SIZE];
    
    while (1) {
        // everytime we need to clear up the buf
        bzero(buf, sizeof(buf));

        // if no read bytes, break the loop 
        // if client quits, server still gets a read but receives 0 bytes
        if (read(connfd, buf, BUFFER_SIZE) == 0) {
            printf("client quit\n");
            break;
        }

        printf("Received: %s", buf);

        // we send back some messages (stored in send_line) to client
        char send_line[MAXLINE];
        sprintf(send_line, "Hi, %s", buf);
        
        int nbytes = sizeof(send_line);
        
        // if write fails, exit
        if (write(connfd, send_line, nbytes) != nbytes) {
            fprintf(stderr, "write error\n");
            exit(1);
        }
    }

    close(listenfd);
    close(connfd);

    return 0;
}
