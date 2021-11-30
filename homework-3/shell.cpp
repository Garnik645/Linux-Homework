#include "shell_lib.h"

int main()
{
    create_shell_folder();
    while(true)
    {
        char** arg;
        if(!get_command(arg))
        {
            continue;
        }
        call_command(arg);
    }
    return 0;
}