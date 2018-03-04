#include "util.h"

pthread_t createThread(thread_function thread,void* arg)
{
	pthread_t tid;
    void *ret;
    int err = pthread_create(&tid, NULL, thread,arg);
    if (err != 0)
    {
        // printf("pthread_create error\n");
    }
	return tid;
}