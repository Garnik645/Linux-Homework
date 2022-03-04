#ifndef SHARED_MEMORY_BUFFER_H
#define SHARED_MEMORY_BUFFER_H

#include <iostream>
#include <string>
#include <cerrno>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

class Buffer {
private:
    sem_t *crit_sync;
    sem_t *empty_sync;
    sem_t *full_sync;
    char *data;
    int capacity;
    int size;

    template<typename T>
    static void check(T value, T error_value, const std::string &func_name) {
        if (value == error_value) {
            std::cerr << "something went wrong with " << func_name << ", error " << errno << '\n';
            exit(errno);
        }
    }

public:
    char pop() {
        check(sem_wait(full_sync), -1, "sem_wait");
        check(sem_wait(crit_sync), -1, "sem_wait");
        char result = data[size--];
        check(sem_post(crit_sync), -1, "sem_post");
        check(sem_post(empty_sync), -1, "sem_post");
        return result;
    }

    void push(char c) {
        check(sem_wait(empty_sync), -1, "sem_wait");
        check(sem_wait(crit_sync), -1, "sem_wait");
        data[size++] = c;
        check(sem_post(crit_sync), -1, "sem_post");
        check(sem_post(full_sync), -1, "sem_post");
    }

    explicit Buffer(int _capacity = getpagesize()) : capacity(_capacity), size(0) {
        int fd = shm_open("/prod-cons-buffer", O_RDWR | O_CREAT, 0777);
        check(fd, -1, "shm_open");

        auto *file_stat = new struct stat;
        check(fstat(fd, file_stat), -1, "fstat");

        crit_sync = sem_open("/prod-cons-buffer-crit-sem", O_RDWR | O_CREAT, 0777, 1);
        check(crit_sync, SEM_FAILED, "sem_open");
        empty_sync = sem_open("/prod-cons-buffer-empty-sem", O_RDWR | O_CREAT, 0777, 0);
        check(empty_sync, SEM_FAILED, "sem_open");
        full_sync = sem_open("/prod-cons-buffer-full-sem", O_RDWR | O_CREAT, 0777, capacity);
        check(full_sync, SEM_FAILED, "sem_open");

        check(sem_wait(crit_sync), -1, "sem_wait");

        if (file_stat->st_size != capacity) {
            check(ftruncate(fd, capacity), -1, "ftruncate");
        }

        check(sem_post(crit_sync), -1, "sem_post");

        void *shm = mmap(nullptr, capacity, PROT_WRITE, MAP_SHARED, fd, 0);
        check(shm, MAP_FAILED, "mmap");
        data = static_cast<char *>(shm);
    }
};

#endif
