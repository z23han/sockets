#include "../common.h"

static int count;

static void recvfrom_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}


int main(int argc, char **argv) {

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    socklen_t client_len;
    
    char message[MAXLINE];
    message[0] = 0;
    count = 0;

    signal(SIGINT, recvfrom_int);
    
    struct sockaddr_in client_addr;
    client_len = sizeof(client_addr);

    // get client info into client_addr
    int n = recvfrom(socket_fd, message, MAXLINE, 0, (struct sockaddr *)&client_addr, &client_len);
    if (n < 0) {
        fprintf(stderr, "recvfrom error\n");
        exit(1);
    }

    message[n] = 0;
    printf("received %d bytes %s\n", n, message);
    
    // bind socket_fd to client_addr ???
    if (connect(socket_fd, (struct sockaddr *)&client_addr, client_len)) {
        fprintf(stderr, "connect failed\n");
        exit(1);
    }

    char send_line[MAXLINE];
    bzero(&message, MAXLINE);

    while (strncmp(message, "goodbye", 7) != 0) {

        bzero(&send_line, MAXLINE);
        
        sprintf(send_line, "Hi, %s", message);
        
        size_t rt = send(socket_fd, send_line, strlen(send_line), 0);
        if (rt < 0) {
            fprintf(stderr, "send failed\n");
            exit(1);
        }

        printf("send %zu bytes\n", rt);

        bzero(message, MAXLINE);
        
        size_t rc = recv(socket_fd, message, MAXLINE, 0);
        if (rc < 0) {
            fprintf(stderr, "recv failed\n");
            exit(1);
        } else if (rc == 0) {
            fprintf(stdout, "client closed\n");
            exit(0);
        }

        count++;
    }

    return 0;
}
