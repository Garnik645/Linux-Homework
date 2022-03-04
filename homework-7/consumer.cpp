#include "buffer.h"

int main() {
    Buffer buff("425");
    char x;
    while (true) {
        std::cout << buff.read(0) << buff.read(1) << buff.read(2) << std::endl;
        sleep(1);
    }
}