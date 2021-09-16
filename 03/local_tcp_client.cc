#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/un.h>


#define MAXLINE 2048

/*
 * this example is based on tcp stream and local_path socket
 * */
int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: ipc_client <local_path>");
        exit(1);
    }

    int sockfd;
    struct sockaddr_un servaddr;

    // construct the client socket fd
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "create socket failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    // fill up the servaddr struct info
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "connect failed\n");
        exit(1);
    }

    // use send_line for sending, and recv_line for receiving from server
    char send_line[MAXLINE];
    bzero(send_line, MAXLINE);
    char recv_line[MAXLINE];

    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        
        // we capture from stdin
        int nbytes = sizeof(send_line);
        
        // if write bytes is smaller, it means we fails to write to the client socket
        if (write(sockfd, send_line, nbytes) != nbytes) {
            fprintf(stderr, "write error\n");
            exit(1);
        }

        // if we didn't read anything from server, server stops
        if (read(sockfd, recv_line, MAXLINE) == 0) {
            fprintf(stderr, "server terminated prematurely\n");
            exit(1);
        }

        // puts what we received to stdout
        fputs(recv_line, stdout);
    }

    return 0;
}
