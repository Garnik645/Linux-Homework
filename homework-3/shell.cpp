#include "shell_lib.h"

int main()
{
    // create /opt/silentshell/ folder
    // root permisson is needed
    create_shell_folder();

    // repeat until the porcess is ended
    while(true)
    {
        // memory for input
        char input[INPUT_SIZE + 1];
        char** arg;

        // get command, return false if no command was given
        if(!get_command(input, arg))
        {
            continue;
        }

        // execute the given command
        exe_command(arg);
    }
    return 0;
}