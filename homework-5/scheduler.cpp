#include "scheduler_lib.h"

struct inc_arg
{
    pthread_mutex_t mt;
    int x;
};

void inc(void* x)
{
    inc_arg* arg = (inc_arg*)x;
    pthread_mutex_lock(&arg->mt);
    ++(arg->x);
    pthread_mutex_unlock(&arg->mt);
}

int main()
{
    parallel_scheduler* sch = new parallel_scheduler(40);
    inc_arg* arg = new inc_arg;
    arg->mt = PTHREAD_MUTEX_INITIALIZER;
    arg->x = 0;
    for(size_t i = 0; i < 10000; ++i)
    {
        sch->run(inc, (void*)arg);
    }
    sleep(1);
    std::cout << arg->x << std::endl;
    return 0;
}