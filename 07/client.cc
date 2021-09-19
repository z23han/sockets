#include "../common.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: client <IPaddress>\n");
        exit(1);
    }

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Here tcp ipv4 server struct is used
    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);

    if (connect(socket_fd, (struct sockaddr *)&server_addr, server_len) < 0) {
        fprintf(stderr, "connect failed");
        exit(1);
    }

    struct sockaddr *reply_addr;
    reply_addr = (sockaddr *)malloc(server_len);

    char send_line[MAXLINE], recv_line[MAXLINE];
    socklen_t len;
    int n;

    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        
        int i = strlen(send_line);
        if (send_line[i-1] == '\n') 
            send_line[i-1] = 0;

        printf("now sending %s\n", send_line);
        
        size_t rt = sendto(socket_fd, send_line, strlen(send_line), 0, (struct sockaddr *)&server_addr, server_len);
        if (rt < 0) {
            fprintf(stderr, "sendto error\n");
            exit(1);
        }

        printf("send bytes %zu \n", rt);
        
        len = 0;
        bzero(&recv_line, MAXLINE);

        n = recvfrom(socket_fd, recv_line, MAXLINE, 0, reply_addr, &len);
        
        if (n < 0) {
            fprintf(stderr, "recvfrom error\n");
            exit(1);
        }

        recv_line[n] = 0;
        fputs(recv_line, stdout);
        fputs("\n", stdout);
    }

    return 0;
}
