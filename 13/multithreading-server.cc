#include "../common.h"
#include <pthread.h>


extern void loop_echo(int);


void *thread_run(void *arg)
{
    pthread_detach(pthread_self());
    int fd = *(int *)arg;

    loop_echo(fd);
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
        fprintf(stderr, "listen error\n");
        exit(1);
    }
    
    pthread_t tid;

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
        else {
            // here we cast the fd to be a pointer and pass it to the thread argv
            pthread_create(&tid, NULL, &thread_run, &fd);
        }
    }
    
    return 0;
}

