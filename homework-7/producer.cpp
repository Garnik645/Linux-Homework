#include "buffer_lib/buffer.h"

int main() {
    Stack q("777");
    while (true) {
        char c;
        std::cin >> c;
        q.push(c);
    }
}