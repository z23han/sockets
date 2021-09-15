#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>


void read_data(int sockfd) {
    
    ssize_t n;
    char buf[1024];

    int time = 0;

    for (;;) {
        fprintf(stdout, "block in read\n");
        if ((n = read(sockfd, buf, 1024)) == 0) 
            return;

        time++;
        fprintf(stdout, "1K read for %d\n", time);
        usleep(100);
    }
}


int main() {
    
    int listenfd, connfd;
    socklen_t clilen;
    
    struct sockaddr_in cliaddr, servaddr;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);
    
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    listen(listenfd, 1024);

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        read_data(connfd);
        close(connfd);
    }

    close(listenfd);

    return 0;
}
