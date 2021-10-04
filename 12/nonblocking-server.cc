#include "../common.h"
#include "buffer.h"


int main(int argc, char **argv)
{
    int listen_fd;
    int i, maxfd;

    struct Buffer *buffer[FD_INIT_SIZE];
    for (i = 0; i < FD_INIT_SIZE; ++i)
    {
        buffer[i] = alloc_Buffer();
    }

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    struct linger ling;
    ling.l_onoff = 1;
    ling.l_linger = 0;
    setsockopt(listen_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
    

    return 0;
}

