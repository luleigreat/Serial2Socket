#pragma once

#include <pthread.h>

typedef void* thread_function(void*);

pthread_t createThread(thread_function thread,void* arg);