#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


/*
 * this blockQueue implements a simple circular buffer, the fd pointer is stored in each node
 * */
typedef struct 
{
    int number; 
    int *fd;
    int front; 
    int rear; 
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} block_queue;


void block_queue_init(block_queue *blockQueue, int number)
{
    blockQueue->number = number;
    blockQueue->fd = (int *)calloc(number, sizeof(int));
    blockQueue->front = blockQueue->rear = 0;
    
    pthread_mutex_init(&blockQueue->mutex, NULL);
    pthread_cond_init(&blockQueue->cond, NULL);
}


void block_queue_push(block_queue *blockQueue,int fd)
{
    pthread_mutex_lock(&blockQueue->mutex);

    blockQueue->fd[blockQueue->rear] = fd;
    
    // if i move forward and find i'm at the end, i need to go back to front
    if (++blockQueue->rear == blockQueue->number)
    {
        blockQueue->rear = 0;
    }
    fprintf(stderr, "push fd %d\n", fd);

    // after I push the item, I need to signal the condition_variable
    pthread_cond_signal(&blockQueue->cond);

    pthread_mutex_unlock(&blockQueue->mutex);
}


int block_queue_pop(block_queue *blockQueue)
{
    pthread_mutex_lock(&blockQueue->mutex);

    // if front=rear, it means no items available so far, I have to wait on the signal here, with mutex
    while (blockQueue->front == blockQueue->rear)
    {
        pthread_cond_wait(&blockQueue->cond, &blockQueue->mutex);
    }
    int fd = blockQueue->fd[blockQueue->front];
    
    // i need to move forward, and if i'm at the end, I need to go back to the front
    if (++blockQueue->front == blockQueue->number)
    {
        blockQueue->front = 0;
    }

    fprintf(stdout, "pop fd %d\n", fd);

    pthread_mutex_unlock(&blockQueue->mutex);

    return fd;
}


#endif // BLOCK_QUEUE_H

