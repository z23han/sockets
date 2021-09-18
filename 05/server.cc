#include "../common.h"
#include "message_object.h"

static int count;


int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: server <sleepingTime>");
        exit(1);
    }

    int sleepingTime = atoi(argv[1]);
    
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    int rt1 = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        fprintf(stderr, "bind error: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        fprintf(stderr, "listen error: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    bzero(&client_addr, client_len);

    if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        fprintf(stderr, "bind failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    messageObject message;
    count = 0;

    for (;;) {
        int n = read(connfd, (char *)&message, sizeof(messageObject));
        
        if (n < 0) {
            fprintf(stderr, "read failed");
            exit(1);
        } else if (n == 0) {
            fprintf(stdout, "client closed");
            exit(0);
        }

        printf("received %d bytes\n", n);
        count++;

        switch (ntohl(message.type)) {
            case MSG_TYPE1:
                printf("process MSG_TYPE1 \n");
                break;

            case MSG_TYPE2:
                printf("process MSG_TYPE2 \n");
                break;

            case MSG_PING: {
                // if it's ping, we construct pong message and send back
                messageObject pong;
                pong.type = MSG_PONG;

                sleep(sleepingTime);
                ssize_t rc = send(connfd, (char *)&pong, sizeof(pong), 0);

                if (rc < 0) {
                    fprintf(stderr, "send fail");
                    exit(1);
                }
                break;
            }

            default:
                fprintf(stderr, "unknow message type\n");
                exit(1);
        }
    }

    return 0;
}


