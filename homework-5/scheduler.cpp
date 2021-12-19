#include "scheduler_lib.h"

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