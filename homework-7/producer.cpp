#include "buffer.h"

int main() {
    Buffer buff("425");
    while(true)
    {
        char c = 'a';
        int i = 0;
        buff.write(i, c);
        sleep(1);
        ++i;
        ++c;
        i %= 3;
    }
}