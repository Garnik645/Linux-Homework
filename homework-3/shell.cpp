#include "shell_lib.h"

int main()
{
    // create /opt/silentshell/ directory
    // root permisson is needed
    create_shell_folder();

    // repeat until the porcess is killed
    while(true)
    {
        // memory for input
        char input[INPUT_SIZE + 1];
        char** arg;

        // get command
        if(!get_command(input, arg))
        {
            // continue if no command was given
            continue;
        }

        // execute the given command
        exe_command(arg);
    }
    return 0;
}