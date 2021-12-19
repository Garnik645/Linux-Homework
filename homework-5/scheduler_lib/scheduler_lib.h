#pragma once
#include <iostream>
#include <pthread.h>
#include <functional>
#include <queue>
#include <unistd.h>

void* consumer(void*);

class parallel_scheduler
{
private:
    size_t thread_count;
    pthread_t* threads;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    std::queue<std::pair<std::function<void(void*)>, void*>> func_queue;
    friend void* consumer(void*);

public:
    parallel_scheduler(size_t);
    void run(std::function<void(void*)>, void*);
    bool done();
    ~parallel_scheduler();
};