#include "../common.h"
#include <iostream>

static void handler(int signo)
{
    std::cout << "SIGPIPE ERROR" << std::endl;
}


int main(int argc, char **argv)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    const char ip[] = "127.0.0.1";

#if 0
    signal(SIGPIPE, handler);
#endif

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);

    int rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    if (rt < 0)
    {
        fprintf(stderr, "conn error\n");
        exit(1);
    }

    fprintf(stdout, "client connected\n");

    char buf[MAXLINE];

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    for (;;) {

        memset(buf, 0, MAXLINE);
        fgets(buf, MAXLINE, stdin);

        int rc = send(socket_fd, buf, strlen(buf), 0);
        
        if (rc < 0)
        {
            fprintf(stderr, "send error\n");
            close(socket_fd);
            exit(1);
        }
        else
        {
            std::cout << "sent " << rc << " bytes: " << buf << std::endl;
        }
        
    }

    return 0;
}


