#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>


#define MAXLINE 4096
#define SERV_PORT 12345


int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: client <IPaddress>");
        exit(1);
    }

    // set up server socket 
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // construct server_addr struct
    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    
    // connect to server 
    int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    if (connect_rt < 0) {
        fprintf(stderr, "connect failed: %s (%d)\n", strerror(errno), errno);
        exit(1);
    }

    char send_line[MAXLINE];
    char recv_line[MAXLINE+1];
    int n;

    // readmask is a temp fd_set
    fd_set readmask;
    fd_set allreads;

    // get ready for select
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);

    // allreads bind to socket_fd
    FD_SET(socket_fd, &allreads);
    
    for (;;) {
        readmask = allreads;
        
        // use socket+1 as select socket ??
        int rc = select(socket_fd+1, &readmask, NULL, NULL, NULL);
        if (rc <= 0) {
            fprintf(stderr, "select failed: %s (%d)", strerror(errno), errno);
            exit(1);
        }


        // we have 2 checks, first check from the server socket, next check from user input
        // if there's data to read from socket (it's from server) 
        if (FD_ISSET(socket_fd, &readmask)) {

            // read data into recv_line
            n = read(socket_fd, recv_line, MAXLINE);

            if (n < 0) {
                fprintf(stderr, "read error: %s (%d)\n", strerror(errno), errno);
                exit(1);
            } else if (n == 0) {
                fprintf(stderr, "server terminated: %s (%d)\n", strerror(errno), errno);
                exit(1);
            }

            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        // reset the readmask 
        if (FD_ISSET(0, &readmask)) {

            // if there's data from stdin (it's from user input) 
            if (fgets(send_line, MAXLINE, stdin) != NULL) {

                // if it's shutdown command
                if (strncmp(send_line, "shutdonw", 8) == 0) {

                    // clear 
                    FD_CLR(0, &allreads);

                    if (shutdown(socket_fd, 1)) {
                        fprintf(stderr, "shutdown failed: %s (%d)\n", strerror(errno), errno);
                        exit(1);
                    }
                } else if (strncmp(send_line, "close", 5) == 0) {// if it's close command 
                    
                    // clear
                    FD_CLR(0, &allreads);

                    if (close(socket_fd)) {
                        fprintf(stderr, "close failed: %s (%d)\n", strerror(errno), errno);
                        exit(1);
                    }
                    sleep(6);
                    exit(0);
                } else { // otherwise it's regular input 
                    
                    int i = strlen(send_line);
                    
                    if (send_line[i-1] == '\n') {
                        send_line[i-1] = 0;
                    }

                    printf("now sending %s\n", send_line);

                    // write back from server socket to server
                    size_t rt = write(socket_fd, send_line, strlen(send_line));
                    if (rt < 0) {
                        fprintf(stderr, "write failed: %s (%d)\n", strerror(errno), errno);
                        exit(1);
                    }
                    printf("send bytes: %zu \n", rt);
                }
            }
        }
    }

    return 0;
}


