#include "../common.h"


int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: client <IPaddress>");
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
        fprintf(stderr, "connect fail\n");
        exit(1);
    }

    char buf[128];
    struct iovec iov[2];
    
    char *send_one = "hello,";
    iov[0].iov_base = send_one;
    iov[0].iov_len = strlen(send_one);
    iov[1].iov_base = buf;

    char message[MAXLINE];
    
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        
        iov[1].iov_len = strlen(buf);
        int n = htonl(iov[1].iov_len);
        
        if (writev(socket_fd, iov, 2) < 0) {
            fprintf(stderr, "writev fail: %s (%d)\n", strerror(errno), errno);
            exit(1);
        }

        bzero(&message, MAXLINE);
        int nbytes = read(socket_fd, message, MAXLINE);

        if (nbytes < 0) {
            fprintf(stderr, "read error\n");
            exit(1);
        } else if (nbytes == 0) {
            fprintf(stdout, "server closed\n");
            exit(0);
        }

        printf("Received %s\n", message);
    }

    return 0;
}
