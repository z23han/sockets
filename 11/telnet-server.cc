#include "../common.h"

#define INIT_SIZE 128

int main(int argc, char **argv)
{
    int listen_fd, conn_fd;

    // initialize server listen_fd
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0)
    {
        fprintf(stderr, "server bind error\n");
        exit(1);
    }

    int rt2 = listen(listen_fd, LISTENQ);
    if (rt2 < 0)
    {
        fprintf(stderr, "server listen failed\n");
        exit(1);
    }

    struct pollfd event_set[INIT_SIZE];
    event_set[0].fd = listen_fd;
    event_set[0].events = POLLRDNORM;

    int i;
    for (i = 1; i < INIT_SIZE; ++i)
    {
        event_set[i].fd = -1;
    }

    int ready_number;
    char buf[MAXLINE];
    ssize_t n;

    struct sockaddr_in client_addr;

    for (;;)
    {
        // first we need to check the accept and get a conn_fd
        // if timeout < 0, it's blocked and waiting for an event here here
        // if timeout = 0, it's non-block 
        if ((ready_number = poll(event_set, INIT_SIZE, -1)) < 0)
        {
            fprintf(stderr, "poll failed\n");
            exit(1);
        }

        // check revents and POLLRDNORM
        if (event_set[0].revents & POLLRDNORM)
        {
            socklen_t client_len = sizeof(client_addr);
            conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
            
            // we need to find one position to store the socket and events
            for (i = 1; i < INIT_SIZE; ++i)
            {
                if (event_set[i].fd < 0)
                {
                    event_set[i].fd = conn_fd;
                    event_set[i].events = POLLRDNORM;
                    break;
                }
            }

            // if means event_set is filled up already
            if (i == INIT_SIZE) 
            {
                fprintf(stderr, "cannot hold so many clients\n");
                exit(1);
            }

            fprintf(stdout, "one client is connected\n");

            // if ready_number is back to 0, we ignore the rest since there's no requests
            if (--ready_number <= 0)
                continue;
        }

        for (i = 1; i < INIT_SIZE; ++i)
        {
            int socket_fd;
            if ((socket_fd = event_set[i].fd) < 0)
                continue;
            
            if (event_set[i].revents & (POLLRDNORM | POLLERR))
            {
                if ((n = read(socket_fd, buf, MAXLINE)) > 0)
                {
                    if (write(socket_fd, buf, n) < 0) 
                    {
                        fprintf(stderr, "write error\n");
                        exit(1);
                    }
                }
                else if (n == 0 || errno == ECONNRESET) 
                {
                    close(socket_fd);
                    event_set[i].fd = -1;
                    fprintf(stdout, "one client is closed\n");
                }
                else
                {
                    fprintf(stderr, "read error\n");
                    exit(1);
                }

                // it means there're no available pollfd spots
                if (--ready_number <= 0)
                    break;
            }
        }
    }

    return 0;
}
