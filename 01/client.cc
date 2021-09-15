#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>


#define MESSAGE_SIZE 102400

void send_data(int sockfd) {
    
    char *query;
    query = (char *)malloc(MESSAGE_SIZE+1);
    
    for (int i = 0; i < MESSAGE_SIZE; ++i) {
        query[i] = 'a';
    }
    query[MESSAGE_SIZE] = '\0';

    const char *cp;
    cp = query;
    size_t remaining = strlen(query);

    while (remaining) {
        
        int n_written = send(sockfd, cp, remaining, 0);
        fprintf(stdout, "send into buffer %d \n", n_written);
        
        if (n_written <= 0) {
            printf("send failed\n");
            printf("error code: %d\n", errno);
            return;
        }

        remaining -= n_written;
        cp += n_written;
    }
    return;
}


int main(int argc, char **argv) {
    
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        printf("usage: tcpclient <IPaddress>");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    int connect_rt = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    if (connect_rt < 0) {
        printf("connect failed\n");
        printf("error code: %d\n", errno);
        exit(1);
    }
    send_data(sockfd);

    close(sockfd);

    return 0;
}

