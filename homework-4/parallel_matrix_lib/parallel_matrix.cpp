#include "parallel_matrix.h"

struct arg
{
    std::vector<std::vector<int>>* data = nullptr;
    size_t rest = 0;
    size_t t = 0;
    arg(std::vector<std::vector<int>>* _data = nullptr, size_t _rest = 0, size_t _t = 0)
        : data(_data)
        , rest(_rest)
        , t(_t)
        {}
};

void* thread_sum(void* in)
{
    int* ans = new int;
    arg* th_arg = (arg*) in;
    std::vector<std::vector<int>>* th_mtrx = th_arg->data;
    size_t col = th_mtrx->size();
    size_t row = (th_mtrx->at(0)).size();
    // for(size_t i = 0; i < col; ++i)
    // {
    //     for(size_t j = 0; j < row; ++j)
    //     {
    //         std::cout << (*th_mtrx)[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;
    size_t el_count = col * row;
    for(size_t l = th_arg->rest; l < el_count; l += th_arg->t)
    {
        size_t i = l / row;
        size_t j = l % row;
        *ans += (*th_mtrx)[i][j];
    }
    return (void*) ans;
}

parallel_matrix::parallel_matrix(size_t n, size_t m)
{
    assert(n > 0 && m > 0);
    mtrx.resize(n);
    for(size_t i = 0; i < n; ++i)
    {
        mtrx[i].resize(m);
    }
}

void parallel_matrix::init()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(-10, 10);
    for(size_t i = 0; i < mtrx.size(); ++i)
    {
        for(size_t j = 0; j < mtrx[i].size(); ++j)
        {
            mtrx[i][j] = dist6(rng);
        }
    }
}

int parallel_matrix::sum()
{
    int result = 0;
    for(size_t i = 0; i < mtrx.size(); ++i)
    {
        for(size_t j = 0; j < mtrx[i].size(); ++j)
        {
            result += mtrx[i][j];
        }
    }
    return result;
}

int parallel_matrix::sum_parallel(size_t t)
{
    assert(t > 0);
    int result = 0;
    std::vector<pthread_t> threads(t);
    std::vector<arg> thread_args(t);
    for(size_t i = 0; i < t; ++i)
    {
        thread_args[i] = arg(&mtrx, i, t);
        int thread_out = pthread_create(&threads[i], NULL, thread_sum, &thread_args[i]);
        if(thread_out != 0)
        {
            std::cerr << "Error while creating thread!" << std::endl;
            exit(thread_out);
        }
    }
    for(size_t i = 0; i < t; ++i)
    {
        void* thread_result;
        int thread_out = pthread_join(threads[i], &thread_result);
        if(thread_out != 0)
        {
            std::cerr << "Error while joining thread!" << std::endl;
            exit(thread_out);
        }
        int* thread_result_int = (int*) thread_result;
        result += *thread_result_int;
        delete thread_result_int;
    }
    return result;
}

void measure_time(std::function<int(int)> func, int x)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "\tresult : " << func(x) << std::endl;  
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "\tmeasured time in microseconds : " << duration.count() << std::endl;
}

void measure_time(std::function<int()> func)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "\tresult : " << func() << std::endl;  
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "\tmeasured time in microseconds : " << duration.count() << std::endl;
}