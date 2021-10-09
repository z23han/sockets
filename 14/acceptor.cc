#include "../common.h"


struct acceptor *acceptor_init(int port)
{
    // use a struct to hold listen_fd and port
    struct acceptor *accpt = malloc(sizeof(struct acceptor));
    accpt->listen_port = port;
    accpt->listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // make non-blocking
    fcntl(accpt->listen_fd, F_SETFL, O_NONBLOCK);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int on = 1;
    setsockopt(accpt->listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    int rt1 = bind(accpt->listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0)
    {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }

    int rt2 = listen(accpt->listen_fd, LISTENQ);
    if (rt2 < 0)
    {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }

    return accpt;
}
