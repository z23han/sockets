#include "../common.h"

/*
 * what we have learned is that we fork() a child process to handle the recv request. We need to make sure close listen_fd the child process, and close fd in the parent process
 *
 **/


char rot13_char(char c)
{
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}


void child_run(int fd)
{
    char outbuf[MAXLINE], inbuf[MAXLINE];
    size_t outbuf_used = 0, inbuf_used = 0;
    ssize_t result;

    while (1)
    {
        // we receive from fd char by char
        char ch;
        result = recv(fd, &ch, 1, 0);

        if (result == 0)
            break;
        else if (result == -1)
        {
            perror("read");
            break;
        }

        if (outbuf_used < sizeof(outbuf))
        {
            outbuf[outbuf_used++] = rot13_char(ch);
        }

        if (inbuf_used < sizeof(inbuf))
        {
            inbuf[inbuf_used++] = ch;
        }

        if (ch == '\n')
        {
            fprintf(stdout, "received: %s", inbuf);
            fprintf(stdout, "converted: %s", outbuf);
            send(fd, outbuf, outbuf_used, 0);
            outbuf_used = 0;
            inbuf_used = 0;
            bzero(outbuf, sizeof(outbuf));
            bzero(inbuf, sizeof(inbuf));

            continue;
        }
    }
}


void sigchld_handler(int sig)
{
    while (waitpid(-1, 0, WNOHANG) > 0);

    return;
}


int main(int argc, char **argv)
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    int rc1 = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rc1 < 0)
    {
        fprintf(stderr, "bind error\n");
        exit(1);
    }

    int rc2 = listen(listen_fd, LISTENQ);
    if (rc2 < 0)
    {
        fprintf(stderr, "listen error \n");
        exit(1);
    }

    signal(SIGCHLD, sigchld_handler);

    while (1)
    {
        struct sockaddr_storage ss;
        socklen_t slen = sizeof(ss);

        int fd = accept(listen_fd, (struct sockaddr *)&ss, &slen);
        
        if (fd < 0)
        {
            fprintf(stderr, "accept failed\n");
            exit(1);
        }

        // child process, it doesn't care about listen_fd, so we can close 
        if (fork() == 0)
        {
            // we close it, but it's not totally closed, it's the counter of the socket that gets minus 1
            close(listen_fd);
            child_run(fd);
            exit(0);
        } else 
        {
            // similarly we close fd in the parent process, because parent doesn't care about fd, it only care about listen_fd
            close(fd);
        }
    }

    return 0;
}





