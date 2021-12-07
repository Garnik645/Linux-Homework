#include "parallel_matrix.h"

int main()
{
    parallel_matrix mtrx(100, 200);
    mtrx.init();
    
    std::cout << "--- mtrx.sum() ---" << std::endl;
    measure_time(std::bind(&parallel_matrix::sum, mtrx));
    
    std::cout << "--- mtrx.sum_parallel(8) ---" << std::endl;
    measure_time(std::bind(&parallel_matrix::sum_parallel, mtrx, 8));
    
    std::cout << "--- mtrx.sum_parallel(16) ---" << std::endl;
    measure_time(std::bind(&parallel_matrix::sum_parallel, mtrx, 16));
    
    std::cout << "--- mtrx.sum_parallel(100) ---" << std::endl;
    measure_time(std::bind(&parallel_matrix::sum_parallel, mtrx, 100));
    
    return 0;
}