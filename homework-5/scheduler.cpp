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
    // get number of threads
    std::cout << "Enter number of threads : ";
    int n;
    std::cin >> n;

    // create parallel scheduler with N threads
    parallel_scheduler* scheduler = new parallel_scheduler(n);

    // argument for increment function
    inc_arg* arg = new inc_arg;
    arg->mt = PTHREAD_MUTEX_INITIALIZER;
    arg->x = 0;

    // run 10000 increment functions
    for(size_t i = 0; i < 10000; ++i)
    {
        scheduler->run(inc, (void*)arg);
    }

    // destroy scheduler
    delete scheduler;
    
    // print the result
    std::cout << arg->x << std::endl;

    // destory mutex
    error_check(pthread_mutex_destroy(&arg->mt));

    // free allocated memory
    delete arg;

    return 0;
}