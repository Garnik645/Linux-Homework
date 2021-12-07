#include "parallel_matrix.h"

int main()
{
    parallel_matrix mtrx(2000, 4000);
    mtrx.init();
    
    std::cout << "--- mtrx.sum() ---" << std::endl;
    measure_time(std::bind(&parallel_matrix::sum, mtrx));
    std::cout << std::endl;
    
    auto ft = std::bind(&parallel_matrix::sum_parallel, mtrx, std::placeholders::_1);

    std::cout << "--- mtrx.sum_parallel(8) ---" << std::endl;
    measure_time(ft, 8);
    std::cout << std::endl;

    std::cout << "--- mtrx.sum_parallel(16) ---" << std::endl;
    measure_time(ft, 16);
    std::cout << std::endl;

    std::cout << "--- mtrx.sum_parallel(100) ---" << std::endl;
    measure_time(ft, 100);
    std::cout << std::endl;
    
    return 0;
}