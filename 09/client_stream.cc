#include "../common.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: tcpclient <IPaddress>\n");
        exit(1);
    }

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    if (connect_rt < 0) {
        fprintf(stderr, "connect failed\n");
        exit(1);
    }

    // struct to hold the type/length/message 
    struct message {
        u_int32_t message_length;
        u_int32_t message_type; 
        char buf[128];
    };

    int n;
    message msg;

    bzero(&msg, sizeof(msg));

    while (fgets(msg.buf, sizeof(msg.buf), stdin) != NULL) {
        // put the messages into buf
        n = strlen(msg.buf);
        msg.message_length = htonl(n);
        msg.message_type = 1;
        
        if (send(socket_fd, (char *)&msg, sizeof(msg.message_length)+sizeof(msg.message_type)+n, 0) < 0) {
            fprintf(stderr, "send error\n");
            exit(1);
        }

        bzero(&msg, sizeof(msg));
    }

    return 0;
}
