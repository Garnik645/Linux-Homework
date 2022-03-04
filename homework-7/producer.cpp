#include "buffer_lib/buffer.h"

int main() {
    Queue q("465");
    while (true) {
        char c;
        std::cin >> c;
        q.push(c);
    }
}