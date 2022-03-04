#include "buffer.h"

template<typename T>
void Buffer::check(T value, T error_value, const std::string &func_name) {
    if (value == error_value) {
        std::cerr << "something went wrong with " << func_name << ", error " << errno << '\n';
        exit(errno);
    }
}

void Buffer::write(int i, char c) {
    if (i >= capacity || i < 0) {
        throw std::out_of_range("buffer size out of range while writing");
    }
    check(sem_wait(crit_sync), -1, "sem_wait");
    data[i] = c;
    check(sem_post(crit_sync), -1, "sem_post");
}

char Buffer::read(int i) {
    if (i >= capacity || i < 0) {
        throw std::out_of_range("buffer size out of range while reading");
    }
    check(sem_wait(crit_sync), -1, "sem_wait");
    char result = data[i];
    check(sem_post(crit_sync), -1, "sem_post");
    return result;
}

Buffer::Buffer(const std::string &id) : capacity(getpagesize()) {
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
    data = (char *) shm;
}

Stack::Stack(const std::string &id) : Buffer(id), super_block(sizeof(int)) {
    std::string empty_name = "/prod-cons-buffer-empty-sem-" + id;
    std::string full_name = "/prod-cons-buffer-full-sem-" + id;
    empty_sync = sem_open(empty_name.c_str(), O_RDWR | O_CREAT, 0777, 0);
    check(empty_sync, SEM_FAILED, "sem_open");
    full_sync = sem_open(full_name.c_str(), O_RDWR | O_CREAT, 0777, capacity - super_block);
    check(full_sync, SEM_FAILED, "sem_open");
}

void Stack::push(char c) {
    check(sem_wait(full_sync), -1, "sem_wait");
    check(sem_wait(crit_sync), -1, "sem_wait");
    int *idata = (int *) data;
    data[super_block + (*idata)++] = c;
    check(sem_post(crit_sync), -1, "sem_post");
    check(sem_post(empty_sync), -1, "sem_post");
}

char Stack::pop() {
    check(sem_wait(empty_sync), -1, "sem_wait");
    check(sem_wait(crit_sync), -1, "sem_wait");
    int *idata = (int *) data;
    char result = data[super_block + --(*idata)];
    check(sem_post(crit_sync), -1, "sem_post");
    check(sem_post(full_sync), -1, "sem_post");
    return result;
}