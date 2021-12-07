#include "parallel_matrix.h"

// arg struct to pass to threads
struct arg
{
    // pointer to matrix
    std::vector<std::vector<int>>* data = nullptr;

    // thread calculates matrix elements with index
    // rest, t + rest, 2 * t + rest, ...
    size_t rest = 0;

    // number of threads
    size_t t = 0;

    // basic constructor
    arg(std::vector<std::vector<int>>* _data = nullptr, size_t _rest = 0, size_t _t = 0)
        : data(_data)
        , rest(_rest)
        , t(_t)
        {}
};

// calculate local sum for each thread
void* thread_sum(void* in)
{
    // store result
    int ans = 0;

    // get input
    arg* th_arg = (arg*) in;
    std::vector<std::vector<int>>* th_mtrx = th_arg->data;
    
    // number of columns
    size_t col = th_mtrx->size();

    // number of rows
    size_t row = (th_mtrx->at(0)).size();

    // number of elements in the matrix
    size_t el_count = col * row;
    
    // go through elements with index
    // rest, t + rest, 2 * t + rest, ...
    // and add to the result
    for(size_t l = th_arg->rest; l < el_count; l += th_arg->t)
    {
        size_t i = l / row;
        size_t j = l % row;
        ans += (*th_mtrx)[i][j];
    }
    return (void*) new int(ans);
}

// matrix constructor
// create a matrix with size N x M
parallel_matrix::parallel_matrix(size_t n, size_t m)
{
    assert(n > 0 && m > 0);
    mtrx.resize(n);
    for(size_t i = 0; i < n; ++i)
    {
        mtrx[i].resize(m);
    }
}

// initialize matrix with random numbers
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

// calculate sum of all the elements in the matrix without additional threads
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

// calculate sum of all the elements in the matrix t threads
int parallel_matrix::sum_parallel(size_t t)
{
    // check if t is a natural number
    assert(t > 0);

    // store the result
    int result = 0;

    // allocate memory for thread ID
    pthread_t* threads = new pthread_t[t];

    // allocate memory for arguments
    arg* thread_args = new arg[t];

    // divide matrix into pieces and create threads
    for(size_t i = 0; i < t; ++i)
    {
        // store argument
        thread_args[i] = arg(&mtrx, i, t);

        // create a thread
        int thread_out = pthread_create(&threads[i], NULL, thread_sum, &thread_args[i]);

        // check for errors
        if(thread_out != 0)
        {
            std::cerr << "Error while creating thread!" << std::endl;
            exit(thread_out);
        }
    }

    // memory to store the result of the local sum of the thread
    void* thread_result;

    // for every thread
    for(size_t i = 0; i < t; ++i)
    {
        // join thread
        int thread_out = pthread_join(threads[i], &thread_result);

        // check for errors
        if(thread_out != 0)
        {
            std::cerr << "Error while joining thread!" << std::endl;
            exit(thread_out);
        }

        // convert void* into int*
        int* thread_result_int = (int*) thread_result;

        // add local sum to the result
        result += *thread_result_int;

        // free allocated memory
        delete thread_result_int;
    }

    // free allocated memory
    delete[] threads;
    delete[] thread_args;

    return result;
}

// function to measure the work time of other functions
void measure_time(std::function<int(int)> func, int x)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "\tresult : " << func(x) << std::endl;  
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "\tmeasured time in microseconds : " << duration.count() << std::endl;
}

// function to measure the work time of other functions
void measure_time(std::function<int()> func)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "\tresult : " << func() << std::endl;  
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "\tmeasured time in microseconds : " << duration.count() << std::endl;
}