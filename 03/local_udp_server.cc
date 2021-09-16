#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>


#define BUFFER_SIZE 2048
#define MAXLINE 2048


int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: local_udp_server <local_path>");
        exit(1);
    }

    // create server socket fd
    int socket_fd;
    socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);

    if (socket_fd < 0) {
        fprintf(stderr, "socket create failed\n");
        exit(1);
    }

    // servaddr and fill up the struct info with local_path
    struct sockaddr_un servaddr;
    
    char *local_path = argv[1];
    unlink(local_path);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, local_path);

    // if bind fails , we exit 
    if (bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "bind failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    char buf[BUFFER_SIZE];
    // define clientaddr for receiving from client side
    struct sockaddr_un client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (1) {
        // everytime we clear up the buf 
        bzero(buf, sizeof(buf));
        
        // receive nothing, then we quit
        if (recvfrom(socket_fd, buf, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len) == 0) {
            fprintf(stdout, "client quit\n");
            break;
        }

        printf("Received %s", buf);
        
        // send back with send_line
        char send_line[MAXLINE];
        bzero(send_line, MAXLINE);
        sprintf(send_line, "Hi, %s", buf);
        
        size_t nbytes = sizeof(send_line);
        printf("now sending %s\n", send_line);

        if (sendto(socket_fd, send_line, nbytes, 0, (struct sockaddr *)&client_addr, client_len) != nbytes) {
            fprintf(stderr, "sendto error: %s (%d)\n", strerror(errno), errno);
            exit(1);
        }
    }

    close(socket_fd);
    
    return 0;
}
