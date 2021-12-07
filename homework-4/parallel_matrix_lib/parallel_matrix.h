#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <random>
#include <pthread.h>
#include <cassert>

struct arg;
void* thread_sum(void* in);

class parallel_matrix
{
private:
    std::vector<std::vector<int>> mtrx;

public:
    parallel_matrix(size_t n, size_t m);
    void init();
    int sum();
    int sum_parallel(size_t thread_num);
};

void measure_time(std::function<int(int)> func, int x);
void measure_time(std::function<int()> func);