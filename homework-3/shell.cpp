#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <string>
#include <vector>

int main()
{
    while(true)
    {
        std::string arg;
        std::getline(std::cin, arg);
        std::string command;
        std::vector<std::string> words{};
        size_t pos = 0;
        arg.push_back(' ');
        while((pos = arg.find(' ')) != std::string::npos)
        {
            words.push_back(arg.substr(0, pos));
            arg.erase(0, pos + 1);
        }
        command = words[0];
        char** arg_list = new char*[words.size()];
        for(int i = 0; i < words.size(); ++i)
        {
            arg_list[i] = new char[words[i].size()];
            for(int j = 0; j < words[i].size(); ++j)
            {
                arg_list[i][j] = words[i][j];
            }
        }
        pid_t child_pid = fork();
        if(child_pid < 0)
        {
            std::cerr << "Something went wrong while cloning the process" << std::endl;
            exit(errno);
        }
        if(child_pid == 0)
        {
            int exe = 0;
            exe = execvp(command.c_str(), arg_list);
            if(exe == -1)
            {
                std::cerr << "Something went wrong while executing a program" << std::endl;
                exit(errno);
            }
        }
        else
        {
            int status;
            waitpid(-1, &status, 0);
        }
    }
}