#include "../common.h"

static int count;

static void sig_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}


int main(int argc, char **argv) {

    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int rt1 = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        fprintf(stderr, "bind failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        fprintf(stderr, "listen failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    signal(SIGINT, sig_int);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    bzero(&client_addr, client_len);

    if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        fprintf(stderr, "accept bind failed\n");
        exit(1);
    }

    char message[MAXLINE];
    count = 0;
    bzero(message, MAXLINE);

    for (;;) {
        int n = read(connfd, message, MAXLINE);

        if (n < 0) {
            fprintf(stderr, "read error\n");
            exit(1);
        } else if (n == 0) {
            fprintf(stdout, "client closed\n");
            exit(0);
        }

        message[n] = 0;

        printf("recevied %d bytes: %s\n", n, message);
        count++;

        bzero(message, MAXLINE);
    }

    return 0;
}
