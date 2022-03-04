#include "buffer_lib/buffer.h"

int main() {
    Queue q("465");
    while (true) {
        std::cout << q.pop() << std::endl;
    }
}