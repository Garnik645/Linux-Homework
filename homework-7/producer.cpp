#include "buffer.h"

int main() {
    Buffer buff;
    char x;
    while (true) {
        std::cin >> x;
        buff.push(x);
    }
}