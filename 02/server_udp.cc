#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>


#define MAXLINE 4096
#define SERV_PORT 12345

static int count;

// this method(handler) is used to generate a summary after signal interrupt
static void recvfrom_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}


int main(int argc, char **argv) {

    // define a socket for server
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    
    // server accepts all the IP 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);
    
    bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    socklen_t client_len;
    char message[MAXLINE];
    count = 0;

    // register the handler to the interrupt
    signal(SIGINT, recvfrom_int);
    
    // define socket for client
    struct sockaddr_in client_addr;
    client_len = sizeof(client_addr);
    
    for (;;) {
        
        // receive messages from client 
        int n = recvfrom(socket_fd, message, MAXLINE, 0, (struct sockaddr *)&client_addr, &client_len);
        message[n] = 0;
        printf("received %d bytes: %s\n", n, message);

        // restruct the message into send_line, and send back to client
        char send_line[MAXLINE];
        sprintf(send_line, "Hi, %s", message);

        sendto(socket_fd, send_line, strlen(send_line), 0, (struct sockaddr *)&client_addr, client_len);

        // increment count each time
        count++;
    }

    return 0;
}
