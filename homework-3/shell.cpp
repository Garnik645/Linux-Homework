#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <vector>
#include <cerrno>

#define INPUT_SIZE 255
struct stat st = {0};

void change_stream(std::string dir, const std::string& filename, int stream, mode_t mode)
{
    if (stat(dir.c_str(), &st) == -1) {
        int dir_out = mkdir(dir.c_str(), 0777);
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder or permission denied" << std::endl;
            exit(errno);
        }
    }
    dir += filename;
    int fd = open(dir.c_str(), mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(fd < 0)
    {
        std::cerr << "Something went wrong while opening or creating the file or permission denied" << std::endl;
        exit(errno);
    }
    int dup_out = dup2(fd, stream);
    if(dup_out < 0)
    {
        std::cerr << "Something went wrong while changing stdin, stdout, stderr streams" << std::endl;
        exit(errno);
    }
}

void create_shell_folder()
{
    if(getuid() != 0)
    {
        std::cerr << "Permission Denied!" << std::endl;
        exit(1);
    }

    if (stat("/opt/", &st) == -1) {
        int dir_out = mkdir("/opt/", 0777);
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder or permission denied" << std::endl;
            exit(errno);
        }
    }

    if (stat("/opt/silentshell/", &st) == -1) {
        int dir_out = mkdir("/opt/silentshell/", 0777);
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder or permission denied" << std::endl;
            exit(errno);
        }
    }
}

bool get_command(char** arg)
{
    char input[INPUT_SIZE + 1];
    std::cout << "# ";
    fgets(input, INPUT_SIZE, stdin);
    if(*input == '\n')
    {
        return false;
    }

    std::vector<char*> arg_v;
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
    if(arg_v.size() == 0)
    {
        return false;
    }
    arg = new char*[arg_v.size()];
    for(size_t i = 0; i < arg_v.size(); ++i)
    {
        arg[i] = arg_v[i];
    }
    return true;
}

void call_command(char** arg)
{
    pid_t parent_pid = getpid();
    if(strcmp("exit", arg[0]) == 0)
    {
        exit(0);
    }
    pid_t child_pid = fork();
    if(child_pid < 0)
    {
        std::cerr << "Something went wrong while cloning the process" << std::endl;
        exit(errno);
    }
    if(child_pid == 0)
    {
        if(strcmp("clear", arg[0]) != 0)
        {
            change_stream("/opt/silentshell/" + std::to_string(parent_pid) + "/", "in.std", 0, O_RDONLY | O_CREAT);
            change_stream("/opt/silentshell/" + std::to_string(parent_pid) + "/", "out.std", 1, O_WRONLY | O_CREAT | O_APPEND);
            change_stream("/opt/silentshell/" + std::to_string(parent_pid) + "/", "err.std", 2, O_WRONLY | O_CREAT | O_APPEND);
        }
        int exe = execvp(arg[0], arg);
        if(exe == -1)
        {
            std::cerr << "Something went wrong while executing a program in the child process" << std::endl;
            exit(errno);
        }
    }
    else
    {
        int status;
        waitpid(child_pid, &status, 0);
    }
}

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