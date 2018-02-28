#pragma once

#include <pthread.h>

typedef void* thread_function(void*);

pthread_t createThread(thread_function thread,void* arg)
{
	pthread_t tid;
    void *ret;
    int err = pthread_create(&tid, NULL, thread,arg);
    if (err != 0)
    {
        perror("pthread_create\n");
    }
	return tid;
}