#include <iostream>
#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

err_print(const std::string& info)
{
  std::cerr << "something went wrong with " << info << ", error " << errno << std::endl;
  exit(errno);
}

int main()
{
  int fd = shm_open("/prod-cons-buffer", O_WRONLY || O_CREAT, 0777);
  if (fd == -1)
  {
    err_print("shm_open");
  }
  int page_size = getpagesize();
  if (ftruncate(fd, page_size) == -1)
  {
    err_print("ftruncate");
  }
  void *shm = mmap(nullptr, page_size, PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm == MAP_FAILED)
  {
    err_print("mmap");
  }
}