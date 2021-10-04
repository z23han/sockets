#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <mutex>

int another_shared = 0;

std::mutex mtx;

static void *thread_run(void *argv)
{
    int *calculator = (int *) argv;

    fprintf(stdout, "hello world, tid = %zu\n", pthread_self());
    
    for (int i = 0; i < 10000; ++i)
    {
        //std::lock_guard<std::mutex> lock(mtx);
        *calculator += 1;
        another_shared += 1;
    }
}


int main(int argc, char **argv)
{
    int calculator;

    pthread_t tid1;
    pthread_t tid2;

    pthread_create(&tid1, NULL, thread_run, &calculator);
    pthread_create(&tid2, NULL, thread_run, &calculator);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    fprintf(stdout, "calculator is %d\n", calculator);
    fprintf(stdout, "another_shared is %d\n", another_shared);

    return 0;
}
