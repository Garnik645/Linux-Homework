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
    char *data;
    int capacity;

    template<typename T>
    static void check(T value, T error_value, const std::string &func_name) {
        if (value == error_value) {
            std::cerr << "something went wrong with " << func_name << ", error " << errno << '\n';
            exit(errno);
        }
    }

public:
    void write(int i, char c) {
        if (i >= capacity || i < 0) {
            throw std::out_of_range("buffer size out of range");
        }
        check(sem_wait(crit_sync), -1, "sem_wait");
        data[i] = c;
        check(sem_post(crit_sync), -1, "sem_post");
    }

    char read(int i) {
        if (i >= capacity || i < 0) {
            throw std::out_of_range("buffer size out of range");
        }
        check(sem_wait(crit_sync), -1, "sem_wait");
        char result = data[i];
        check(sem_post(crit_sync), -1, "sem_post");
        return result;
    }

    explicit Buffer(const std::string &id) : capacity(getpagesize()) {
        std::string buffer_name = "/prod-cons-buffer-" + id;
        std::string crit_name = "/prod-cons-buffer-crit-sem-" + id;
        int fd = shm_open(buffer_name.c_str(), O_RDWR | O_CREAT, 0777);
        check(fd, -1, "shm_open");

        struct stat file_stat{};
        check(fstat(fd, &file_stat), -1, "fstat");

        crit_sync = sem_open(crit_name.c_str(), O_RDWR | O_CREAT, 0777, 1);
        check(crit_sync, SEM_FAILED, "sem_open");

        check(sem_wait(crit_sync), -1, "sem_wait");

        if (file_stat.st_size != capacity) {
            check(ftruncate(fd, capacity), -1, "ftruncate");
        }

        check(sem_post(crit_sync), -1, "sem_post");

        void *shm = mmap(nullptr, capacity, PROT_WRITE, MAP_SHARED, fd, 0);
        check(shm, MAP_FAILED, "mmap");
        data = static_cast<char *>(shm);
        std::cout << "Buffer successfully constructed" << std::endl;
    }
};

#endif
