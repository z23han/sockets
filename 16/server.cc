#include "../common.h"
#include <thread>
#include <iostream>

int main()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);
    
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
        fprintf(stderr, "server listen error\n");
        exit(1);
    }

    auto handler = [](int conn_fd)
    {
        auto tid = std::this_thread::get_id();
        std::cout << "tid: " << tid << std::endl;

        char buf[MAXLINE];

        for (;;) {

            memset(buf, 0, MAXLINE);
            int rc = recv(conn_fd, buf, MAXLINE, 0);

            if (rc < 0)
            {
                fprintf(stderr, "recv error, %s\n", strerror(errno));
                exit(1);
            }
            else if (rc == 0)
            {
                std::cout << "client closed, tid=" << tid << std::endl;
                break;
            }
            else
            {
                std::cout << tid << " received: " << buf << std::endl;
            }
        }
    };
    
    for (;;)
    {
        struct sockaddr_in client_addr;

        socklen_t client_len = sizeof(client_addr);

        fprintf(stdout, "waiting for client...\n");

        int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        
        if (conn_fd < 0)
        {
            fprintf(stderr, "accept failed\n");
            exit(1);
        }

        fprintf(stdout, "one client connected, conn_fd=%d\n", conn_fd);

        std::thread t {handler, conn_fd};
        t.detach();
    }

    return 0;
}
