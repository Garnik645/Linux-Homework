#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <cerrno>

#define INPUT_SIZE 256
struct stat st = {0};

void change_stream(std::string dir, const std::string& filename, int stream, mode_t mode)
{
    if (stat(dir.c_str(), &st) == -1) {
        int dir_out = mkdir(dir.c_str(), 0777);
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder or permisson denied" << std::endl;
            exit(errno);
        }
    }
    dir += filename;
    int fd = open(dir.c_str(), mode, S_IRUSR | S_IWUSR);
    if(fd < 0)
    {
        std::cerr << "Something went wrong while opening or creating the file or permisson denied" << std::endl;
        exit(errno);
    }
    dup2(stream, fd);
}

int main()
{
    if (stat("/opt/silentshell/", &st) == -1) {
        int dir_out = mkdir("/opt/silentshell/", 0777);
        if(dir_out < 0)
        {
            std::cerr << "Something went wrong while creating the folder or permisson denied" << std::endl;
            exit(errno);
        }
    }

    while(true)
    {
        char input[INPUT_SIZE + 1];
        char** arg;

        std::cout << (getuid() == 0 ? "# " : "$ ");
        fgets(input, INPUT_SIZE, stdin);
        if(*input == '\n')
        {
            continue;
        }

        std::vector<char*> arg_v;
        for(char *ptr = input; *ptr; ++ptr)
        {
            if(*ptr == ' ')
            {
                continue;
            }
            if(*ptr == '\n')
            {
                break;
            }
            for(arg_v.push_back(ptr); *ptr && *ptr != ' ' && *ptr != '\n'; ++ptr);
            *ptr = '\0';
        }
        arg = new char*[arg_v.size()];
        for(size_t i = 0; i < arg_v.size(); ++i)
        {
            arg[i] = arg_v[i];
        }

        pid_t child_pid = fork();
        if(child_pid < 0)
        {
            std::cerr << "Something went wrong while cloning the process" << std::endl;
            exit(errno);
        }
        if(child_pid == 0)
        {
            change_stream("/opt/silentshell/" + std::to_string(getpid()) + "/", "in.std", 0, O_TRUNC | O_RDONLY | O_CREAT);
            change_stream("/opt/silentshell/" + std::to_string(getpid()) + "/", "out.std", 1, O_TRUNC | O_WRONLY | O_CREAT);
            change_stream("/opt/silentshell/" + std::to_string(getpid()) + "/", "err.std", 2, O_TRUNC | O_WRONLY | O_CREAT);
            int exe = execvp(arg[0], arg);
            std::cout << "execvp return value: " << exe << std::endl;
            if(exe == -1)
            {
                std::cerr << "Something went wrong while executing a program in the child process" << std::endl;
                exit(errno);
            }
            return 0;
        }
        else
        {
            int status;
            waitpid(child_pid, &status, 0);
        }
    }
}