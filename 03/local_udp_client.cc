#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 2048

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: local_udp_client <local_path>");
        exit(1);
    }

    int socket_fd;
    socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);

    if (socket_fd < 0) {
        fprintf(stderr, "create socket failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    struct sockaddr_un client_addr, server_addr;
    
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sun_family = AF_LOCAL;
    // we need tmpnam for the client_addr.sun_path
    strcpy(client_addr.sun_path, tmpnam(NULL));

    if (bind(socket_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        fprintf(stderr, "bind error: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, argv[1]);

    char send_line[MAXLINE];
    bzero(send_line, MAXLINE);

    char recv_line[MAXLINE];
    
    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        
        int len = strlen(send_line);
        if (send_line[len-1] == '\n')
            send_line[len-1] = 0;

        size_t nbytes = strlen(send_line);
        fprintf(stdout, "now sending %s", send_line);

        if (sendto(socket_fd, send_line, nbytes, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) != nbytes) {

            fprintf(stderr, "sendto error: %s (%d)\n", strerror(errno), errno);
            exit(1);
        }

        int n = recvfrom(socket_fd, recv_line, MAXLINE, 0, NULL, NULL);
        recv_line[0] = 0;
        
        fputs(recv_line, stdout);
        fputs("\n", stdout);
    }

    return 0;
}
