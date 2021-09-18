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


#define SERV_PORT 12345
#define MAXLINE 4096
#define LISTENQ 1024


static int count;

static void sig_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}


int main(int argc, char **argv) {
    
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    
    // initialize server struct
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    // bind to listenfd
    int rt1 = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        fprintf(stderr, "bind failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    // listen on the socket
    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        fprintf(stderr, "listen failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    // set up signal handler
    signal(SIGINT, sig_int);
    // 
    signal(SIGPIPE, SIG_IGN);
    
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // accept from client and get the connect socket
    if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        fprintf(stderr, "accept failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    char message[MAXLINE];
    count = 0;

    for (;;) {
        
        // every time read from connect socket 
        int n = read(connfd, message, MAXLINE);
        
        if (n < 0) {
            fprintf(stderr, "read failed: %s (%d)", strerror(errno), errno);
            exit(1);
        } else if (n == 0) {
            // if 0, it means client closes
            fprintf(stdout, "client closed");
            exit(0);
        }

        message[n] = 0;
        printf("received %d bytes: %s\n", n, message);
        
        count++;

        // set up reply messages
        char send_line[MAXLINE];
        sprintf(send_line, "Hi, %s", message);

        // send to client 
        size_t write_nc = send(connfd, send_line, strlen(send_line), 0);

        printf("send bytes: %zu \n", write_nc);
        if (write_nc < 0) {
            fprintf(stderr, "error write: %s (%d)", strerror(errno), errno);
            exit(1);
        }
    }

    return 0;
}

