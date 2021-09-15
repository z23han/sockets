#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


#define MAXLINE 4096
#define SERV_PORT 12345

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("usage: client_udp <IPaddress>\n");
        exit(1);
    }

    // define the server socket, the IP is obtained from user input
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    
    // define client reply socket address IPV4 
    struct sockaddr *reply_addr;
    reply_addr = (sockaddr *)malloc(server_len);
    
    // recv_line is from server, send_line is sent from client 
    char send_line[MAXLINE], recv_line[MAXLINE+1];
    socklen_t len;
    int n;

    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        
        int i = strlen(send_line);
        if (send_line[i-1] == '\n') {
            send_line[i-1] = 0;
        }

        // get send_line from user input 
        printf("now sending %s\n", send_line);

        // send the user input to server
        size_t rt = sendto(socket_fd, send_line, strlen(send_line), 0, (struct sockaddr *)&server_addr, server_len);
        
        if (rt < 0) {
            printf("send error\n");
            exit(1);
        }

        printf("send bytes: %zu \n", rt);

        len = 0;

        // receive from server response, and print it out
        // TODO: recvfrom is blocked, we should make it non-block or time-out. An alternative solution is to start a new thread to recvfrom server.
        // For timeout approach, we can add a confirmation request to server, if after certain time, client doesn't receive anything from server
        n = recvfrom(socket_fd, recv_line, MAXLINE, 0, reply_addr, &len);
        
        if (n < 0) {
            printf("recvfrom failed\n");
            exit(1);
        }
        
        recv_line[n] = 0;
        fputs(recv_line, stdout);
        fputs("\n", stdout);
    }

    return 0;
}

