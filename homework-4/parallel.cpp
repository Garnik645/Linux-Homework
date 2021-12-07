#include "parallel_matrix.h"

int main()
{
    // create a matrix of size 2000 x 4000
    parallel_matrix mtrx(2000, 4000);
    
    // initialize matrix with random numbers
    mtrx.init();
    
    // calculate mtrx.sum() and measure time
    std::cout << "--- mtrx.sum() ---" << std::endl;
    measure_time(std::bind(&parallel_matrix::sum, mtrx));
    std::cout << std::endl;
    
    // bind and generate a function class to pass to other function to measure time of their work
    auto ft = std::bind(&parallel_matrix::sum_parallel, mtrx, std::placeholders::_1);

    // calculate mtrx.sum_parallel(8) and measure time
    std::cout << "--- mtrx.sum_parallel(8) ---" << std::endl;
    measure_time(ft, 8);
    std::cout << std::endl;

    // calculate mtrx.sum_parallel(16) and measure time
    std::cout << "--- mtrx.sum_parallel(16) ---" << std::endl;
    measure_time(ft, 16);
    std::cout << std::endl;

    // calculate mtrx.sum_parallel(100) and measure time
    std::cout << "--- mtrx.sum_parallel(100) ---" << std::endl;
    measure_time(ft, 100);
    std::cout << std::endl;

    return 0;
}