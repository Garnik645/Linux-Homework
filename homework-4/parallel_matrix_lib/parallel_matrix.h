#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>

class parallel_matrix
{
private:
    std::vector<std::vector<int>> mtrx;
public:
    parallel_matrix(int n, int m);
    void init();
    int sum();
    int sum_parallel(int t);
};

void measure_time(std::function<int(int)> func, int x);
void measure_time(std::function<int()> func);