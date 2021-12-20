#include "scheduler_lib.h"

// example function argument struct
struct inc_arg
{
    pthread_mutex_t mt;
    int x;
};

// example function (increment)
void inc(void* x)
{
    inc_arg* arg = (inc_arg*)x;
    error_check(pthread_mutex_lock(&arg->mt));
    ++(arg->x);
    error_check(pthread_mutex_unlock(&arg->mt));
}

int main()
{
    // parallel scheduler with 40 threads was created
    parallel_scheduler* scheduler = new parallel_scheduler(40);

    // argument for increment function
    inc_arg* arg = new inc_arg;
    arg->mt = PTHREAD_MUTEX_INITIALIZER;
    arg->x = 0;

    // run 10000 increment functions
    for(size_t i = 0; i < 10000; ++i)
    {
        scheduler->run(inc, (void*)arg);
    }

    // wait 1 second
    sleep(1);

    // print the result
    std::cout << arg->x << std::endl;

    // free allocated memory
    delete arg;
    delete scheduler;

    return 0;
}