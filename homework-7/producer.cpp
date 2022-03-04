#include "buffer.h"

int main() {
    Buffer buff("425");
    char c = 'a';
    int i = 0;
    while (true) {
        buff.write(i, c);
        sleep(1);
        ++i;
        ++c;
        i %= 3;
    }
}