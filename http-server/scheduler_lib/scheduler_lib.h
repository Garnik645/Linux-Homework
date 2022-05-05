#pragma once
#include <iostream>
#include <pthread.h>
#include <functional>
#include <queue>

class parallel_scheduler {
private:
  size_t thread_count;
  pthread_t *threads;
  pthread_mutex_t *mutex;
  pthread_cond_t *cond;
  bool is_working;
  std::queue<std::pair<std::function<void(void *)>, void *>> func_queue;
  static void error_check(int);
  static void *consumer(void *);

public:
  explicit parallel_scheduler(size_t);
  ~parallel_scheduler();
  void run(const std::function<void(void *)> &, void *);
};