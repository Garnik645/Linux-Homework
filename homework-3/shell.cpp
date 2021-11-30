#include "shell_lib.h"

int main()
{
    // create /opt/silentshell/ folder
    // root permisson is needed
    create_shell_folder();
    while(true)
    {
        char input[INPUT_SIZE + 1];
        char** arg;
        // get command, returns false if no command was given
        if(!get_command(input, arg))
        {
            continue;
        }
        // try to execute the given command
        exe_command(arg);
    }
    return 0;
}