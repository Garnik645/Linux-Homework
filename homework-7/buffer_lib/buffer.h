#ifndef SHARED_MEMORY_BUFFER_H
#define SHARED_MEMORY_BUFFER_H

#include <iostream>
#include <exception>
#include <string>
#include <cerrno>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

class Buffer {
protected:
    sem_t *crit_sync;
    char *data;
    int capacity;

    template<typename T>
    static void check(T value, T error_value, const std::string &func_name);

public:
    explicit Buffer(const std::string &id);

    void write(int i, char c);

    char read(int i);
};

class Stack : Buffer {
    int super_block;
    sem_t *empty_sync;
    sem_t *full_sync;

public:
    explicit Stack(const std::string &id);

    void push(char c);

    char pop();
};

#endif
