#include <iostream>
#include <pthread.h>
#include <functional>
#include <queue>

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
        pthread_mutex_unlock(data->mutex);
        func(func_arg);
    }
}

class parallel_scheduler
{
private:
    size_t thread_count;
    pthread_t* threads;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    std::queue<std::pair<std::function<void(void*)>, void*>> func_queue;
    friend void* consumer(void* arg);

public:
    parallel_scheduler(size_t _thread_count)
    : thread_count(_thread_count)
    , threads(new pthread_t[_thread_count])
    {
        pthread_mutex_init(mutex, NULL);
        pthread_cond_init(cond, NULL);
        for(size_t i = 0; i < thread_count; ++i)
        {
            int thread_out = pthread_create(&threads[i], NULL, consumer, (void*)this);
            if(thread_out != 0)
            {
                std::cerr << "ERROR while creating thread!" << std::endl;
                exit(thread_out);
            }
        }
    }

    void run(std::function<void(void*)> func, void* arg)
    {
        pthread_mutex_lock(mutex);
        func_queue.push(std::make_pair(func, arg));
        pthread_cond_signal(cond);
        pthread_mutex_unlock(mutex);
    }

    ~parallel_scheduler()
    {
        for(size_t i = 0; i < thread_count; ++i)
        {
            pthread_join(threads[i], NULL);
        }
        pthread_cond_destroy(cond);
        pthread_mutex_destroy(mutex);
        delete[] threads;
    }
};

void inc(void* x)
{
    int* y = (int*)x;
    ++(*y);
}

int main()
{
    parallel_scheduler* sch = new parallel_scheduler(16);
    int* num = new int(0);
    for(size_t i = 0; i < 1000; ++i)
    {
        sch->run(inc, (void*)num);
    }
    std::cout << *num << std::endl;
    return 0;
}