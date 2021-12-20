#include "scheduler_lib.h"

// check for errors
void error_check(int out)
{
    if(out != 0)
    {
        std::cerr << "ERROR : " << out << std::endl;
        exit(out);
    }
}

// every thread represnt a consumer which will run the function in the queue
void* consumer(void* arg)
{
    // get arguments
    parallel_scheduler* data = (parallel_scheduler*)arg;

    // function in the queue
    std::function<void(void*)> func;

    // arguments for the function in the queue
    void* func_arg;

    while(data->is_working)
    {
        // lock
        error_check(pthread_mutex_lock(data->mutex));

        // wait if queue is empty
        while(data->func_queue.empty())
        {
            error_check(pthread_cond_wait(data->cond, data->mutex));
            
            // check if work is done
            if(!data->is_working)
            {
                error_check(pthread_mutex_unlock(data->mutex));
                return nullptr;
            }
        }

        // get function and argument from the queue
        func = data->func_queue.front().first;
        func_arg = data->func_queue.front().second;
        data->func_queue.pop();

        // unlock
        error_check(pthread_mutex_unlock(data->mutex));

        // call the function
        func(func_arg);
    }
    return nullptr;
}

// constructor which gets number of threads (consumers)
parallel_scheduler::parallel_scheduler(size_t _thread_count)
: thread_count(_thread_count)
, threads(new pthread_t[_thread_count])
, mutex(new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER))
, cond(new pthread_cond_t(PTHREAD_COND_INITIALIZER))
, is_working(true)
{
    // create threads
    for(size_t i = 0; i < thread_count; ++i)
    {
        error_check(pthread_create(&threads[i], NULL, consumer, (void*)this));
    }
}

// destructor
parallel_scheduler::~parallel_scheduler()
{
    // finish thread work to be able to join them
    is_working = false;

    // wake up threads
    error_check(pthread_cond_broadcast(cond));

    // join threads
    for(size_t i = 0; i < thread_count; ++i)
    {
        error_check(pthread_join(threads[i], NULL));
    }

    // destory conditional variable and mutex
    error_check(pthread_cond_destroy(cond));
    error_check(pthread_mutex_destroy(mutex));

    // free allocated memory
    delete[] threads;
}

// this function will represent a producer which will add function to the queue
void parallel_scheduler::run(std::function<void(void*)> func, void* arg)
{
    // lock
    error_check(pthread_mutex_lock(mutex));
    
    // add function to the queue
    func_queue.push(std::make_pair(func, arg));

    // signal that queue is not empty
    error_check(pthread_cond_signal(cond));
    
    // unlock
    error_check(pthread_mutex_unlock(mutex));
}