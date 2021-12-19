#include "scheduler_lib.h"

void* consumer(void* arg)
{
    parallel_scheduler* data = (parallel_scheduler*)arg;
    std::function<void(void*)> func;
    void* func_arg;
    while(true)
    {
        pthread_mutex_lock(data->mutex);
        while(data->func_queue.empty())
        {
            pthread_cond_wait(data->cond, data->mutex);
        }
        func = data->func_queue.front().first;
        func_arg = data->func_queue.front().second;
        data->func_queue.pop();
        std::cout << "REMOVED" << std::endl;
        std::cout << "QUEUE SIZE : " << data->func_queue.size() << std::endl;
        pthread_mutex_unlock(data->mutex);
        func(func_arg);
    }
}

parallel_scheduler::parallel_scheduler(size_t _thread_count)
: thread_count(_thread_count)
, threads(new pthread_t[_thread_count])
, mutex(new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER))
, cond(new pthread_cond_t(PTHREAD_COND_INITIALIZER))
{
    for(size_t i = 0; i < thread_count; ++i)
    {
        int out = pthread_create(&threads[i], NULL, consumer, (void*)this);
        if(out != 0)
        {
            std::cerr << "ERROR while creating thread!" << std::endl;
            exit(out);
        }
    }
}

void parallel_scheduler::run(std::function<void(void*)> func, void* arg)
{
    pthread_mutex_lock(mutex);
    func_queue.push(std::make_pair(func, arg));
    std::cout << "ADDED" << std::endl;
    std::cout << "QUEUE SIZE : " << func_queue.size() << std::endl;
    pthread_cond_signal(cond);
    pthread_mutex_unlock(mutex);
}

bool parallel_scheduler::done()
{
    return func_queue.empty();
}

parallel_scheduler::~parallel_scheduler()
{
    for(size_t i = 0; i < thread_count; ++i)
    {
        int out = pthread_join(threads[i], NULL);
        if(out != 0)
        {
            std::cerr << "ERROR while joining thread!" << std::endl;
            exit(out);
        }
    }
    pthread_cond_destroy(cond);
    pthread_mutex_destroy(mutex);
    delete[] threads;
}