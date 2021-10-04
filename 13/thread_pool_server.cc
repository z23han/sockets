#include "../common.h"
#include <pthread.h>
#include "block_queue.h"


#define BLOCK_QUEUE_SIZE 100
#define THREAD_NUMBER 8


struct Thread
{
    pthread_t thread_tid;
    long thread_count;
};

Thread *thread_array;


extern void loop_echo(int);


void *thread_run(void *arg)
{
    pthread_t tid = pthread_self();
    pthread_detach(tid);

    block_queue *blockQueue = (block_queue *)arg;
    
    while (1)
    {
        // it's blocked here, until we push one fd and signal
        int fd = block_queue_pop(blockQueue);
        fprintf(stdout, "get fd in thread, fd=%d, tid=%zu\n", fd, tid);
        loop_echo(fd);
    }
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
    
    block_queue blockQueue;
    block_queue_init(&blockQueue, BLOCK_QUEUE_SIZE);

    thread_array = (Thread *)calloc(THREAD_NUMBER, sizeof(Thread));
    
    for (int i = 0; i < THREAD_NUMBER; ++i) 
    {
        pthread_create(&(thread_array[i].thread_tid), NULL, &thread_run, (void *)&blockQueue);
    }

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
            block_queue_push(&blockQueue, fd);
        }
    }
    
    return 0;
}

