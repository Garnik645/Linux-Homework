#include "shell_lib.h"

// directory check status
struct stat st = {0};

// change child process stdin, stdout, stderr stream into given path
void change_stream(std::string dir, const std::string& filename, int stream, mode_t mode)
{
    // check if directory doesn't exist, create one
    if (stat(dir.c_str(), &st) == -1) {
        int dir_out = mkdir(dir.c_str(), 0777);

        // check for errors while creating the directory
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder" << std::endl;
            exit(errno);
        }
    }
    // add filename to the path
    dir += filename;

    // open the file
    int fd = open(dir.c_str(), mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    
    // check for errors while opening the file
    if(fd < 0)
    {
        std::cerr << "Something went wrong while opening or creating the file" << std::endl;
        exit(errno);
    }

    // change the stream into the file
    int dup_out = dup2(fd, stream);

    // check for errors while changing the stream
    if(dup_out < 0)
    {
        std::cerr << "Something went wrong while changing stdin, stdout, stderr streams" << std::endl;
        exit(errno);
    }
}

// create the folder /opt/silentshell/ directory, where all the child process streams whould be directed
void create_shell_folder()
{
    // check for root permission
    if(getuid() != 0)
    {
        std::cerr << "Permission Denied!" << std::endl;
        exit(1);
    }

    // check if /opt/ directory doesn't exist, create
    if (stat("/opt/", &st) == -1) {
        int dir_out = mkdir("/opt/", 0777);

        // check for errors while creating the directory
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder" << std::endl;
            exit(errno);
        }
    }

    // check if /opt/silentshell/ directory doesn't exist, create
    if (stat("/opt/silentshell/", &st) == -1) {
        int dir_out = mkdir("/opt/silentshell/", 0777);

        // check for errors while creating the directory
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder" << std::endl;
            exit(errno);
        }
    }
}

// get input and split into command and arguments
// return false if no command was given
// return true otherwise
bool get_command(char* input, char** arg)
{
    std::cout << "# ";

    // get input
    fgets(input, INPUT_SIZE, stdin);
    
    // check if empty input was given
    if(*input == '\n')
    {
        return false;
    }

    std::vector<char*> arg_v;
    
    // split input into arguments
    for(char *ptr = input; *ptr; ++ptr)
    {
        if(*ptr == ' ' || *ptr == '\t')
        {
            continue;
        }
        if(*ptr == '\n')
        {
            break;
        }
        for(arg_v.push_back(ptr); *ptr && *ptr != ' ' && *ptr != '\n' && *ptr != '\t'; ++ptr);
        *ptr = '\0';
    }

    // check if no command was given
    if(arg_v.size() == 0)
    {
        return false;
    }

    // save arguments into char**
    arg = new char*[arg_v.size()];
    for(size_t i = 0; i < arg_v.size(); ++i)
    {
        arg[i] = arg_v[i];
    }
    return true;
}

// execute a command
void exe_command(char** arg)
{
    // get parent pid
    pid_t parent_pid = getpid();

    // end the process if "exit" command is given
    if(strcmp("exit", arg[0]) == 0)
    {
        exit(0);
    }

    // clone the process
    pid_t child_pid = fork();
    
    // check for errors while cloning
    if(child_pid < 0)
    {
        std::cerr << "Something went wrong while cloning the process" << std::endl;
        exit(errno);
    }

    // do in child process
    if(child_pid == 0)
    {
        // change stdin, stdout, stderr streams only if "clear" command was not given
        if(strcmp("clear", arg[0]) != 0)
        {
            change_stream("/opt/silentshell/" + std::to_string(parent_pid) + "/", "in.std", 0, O_RDONLY | O_CREAT);
            change_stream("/opt/silentshell/" + std::to_string(parent_pid) + "/", "out.std", 1, O_WRONLY | O_CREAT | O_APPEND);
            change_stream("/opt/silentshell/" + std::to_string(parent_pid) + "/", "err.std", 2, O_WRONLY | O_CREAT | O_APPEND);
        }

        // execute the command
        int exe = execvp(arg[0], arg);
        
        // check for errors while executing
        if(exe == -1)
        {
            std::cerr << "Something went wrong while executing a program in the child process" << std::endl;
            exit(errno);
        }
    }
    // do in parent process
    else
    {
        // wait for the child process to end
        int status;
        waitpid(child_pid, &status, 0);
        
        // free allocated memory
        delete [] arg;
    }
}
