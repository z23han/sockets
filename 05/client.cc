#include "../common.h"
#include "message_object.h"

#define KEEP_ALIVE_TIME  10
#define KEEP_ALIVE_INTERVAL 3
#define KEEP_ALIVE_PROBETIMES   3


int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: client <IPaddress>");
        exit(1);
    }

    // construct server socket
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // construct ipv4 address
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    
    // try connecting to server
    int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    if (connect_rt < 0) {
        fprintf(stderr, "connect faild: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    char recv_line[MAXLINE];
    int n; 

    // define select variables
    fd_set readmask;
    fd_set allreads;

    // define heartbeats
    struct timeval tv;
    int heartbeats = 0;

    tv.tv_sec = KEEP_ALIVE_TIME;
    tv.tv_usec = 0;
    
    messageObject messageObject;
    
    FD_ZERO(&allreads);
    FD_SET(socket_fd, &allreads);

    for (;;) {
        readmask = allreads;

        // client starts select 
        int rc = select(socket_fd+1, &readmask, NULL, NULL, &tv);
        if (rc < 0) {
            fprintf(stderr, "select failed: %s (%d)\n", strerror(errno), errno);
            exit(1);
        }

        // if the return value is 0, it reaches KEEP_ALIVE_TIME
        if (rc == 0) {
            if (++heartbeats > KEEP_ALIVE_PROBETIMES) {
                fprintf(stderr, "connection dead\n");
                exit(1);
            }

            // client doesn't get anything from server, so clients sends PING
            printf("sending heartbeat #%d\n", heartbeats);
            
            messageObject.type = htonl(MSG_PING);

            rc = send(socket_fd, (char *)&messageObject, sizeof(messageObject), 0);
            if (rc < 0) {
                fprintf(stderr, "send failed %s (%d)\n", strerror(errno), errno);
                exit(1);
            }
            tv.tv_sec = KEEP_ALIVE_INTERVAL;
            continue;
        }

        // client receives messages from server
        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAXLINE);
            if (n < 0) {
                fprintf(stderr, "read error");
                exit(1);
            } else if (n == 0) {
                fprintf(stdout, "server terminated\n");
                exit(0);
            }

            printf("received heartbeat, make heartbeat to 0\n");

            // reset heartbeats
            heartbeats = 0;

            // since client gets server response, reset the tv_sec 
            tv.tv_sec = KEEP_ALIVE_TIME;
        }
    }

    return 0;
}
