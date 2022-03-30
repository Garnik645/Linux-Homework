#include <iostream>
#include <vector>
#include <random>

#include <sys/wait.h>
#include <unistd.h>

// struct for input
struct input {
    int buffer_size;
    int children_number;
};

// get arguments, check for validness
input get_arg(int argc, char **argv) {
    // check number of argument given
    if (argc > 3) {
        throw std::invalid_argument("Too many arguments");
    }
    if (argc < 3) {
        throw std::invalid_argument("Too few arguments");
    }

    // convert char* to std::string
    std::string first_arg(argv[1]);
    std::string second_arg(argv[2]);

    // std::string to int
    int size = std::stoi(first_arg);
    int number = std::stoi(second_arg);

    // check some limits
    if (size < 1000000 || number > 1000 || size % number != 0) {
        throw std::invalid_argument("Buffer size or Children number is invalid");
    }

    return {size, number};
}

// initialize buffer with random numbers
void init_rand(std::vector<int> &buff) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(-100, 100);
    for (int &i: buff) {
        i = dist6(rng);
    }
}

// get resulting total sum
int get_total(const std::vector<int> &pipes) {
    // wait for children
    int out = waitpid(-1, nullptr, 0);
    if (out == -1) {
        throw std::runtime_error("waitpid failure");
    }

    int total = 0;
    // for every child get resulting subtotal sum
    // from pipes and add to total
    for (int pipe: pipes) {
        int sub_total;
        auto rdout = read(pipe, &sub_total, sizeof(int));
        if (rdout == -1) {
            throw std::runtime_error("read failure");
        }
        total += sub_total;
    }
    return total;
}

// functionality for child processes :
// read buffer range from input pipe,
// calculate subtotal sum and write into output pipe
void child(const std::vector<int> &buff, int read_fd, int write_fd) {
    int sum = 0;
    int range[2];
    auto rdout = read(read_fd, range, sizeof(int) * 2);
    if (rdout == -1) {
        throw std::runtime_error("read failure");
    }
    for (int i = range[0]; i < range[1]; ++i) {
        sum += buff[i];
    }
    std::cout << "Sub-Total : " << sum << std::endl;
    auto wrout = write(write_fd, &sum, sizeof(int));
    if (wrout == -1) {
        throw std::runtime_error("write failure");
    }
}

int main(int argc, char **argv) {
    // get arguments
    input arg = get_arg(argc, argv);

    // slice buffer into smaller parts for children to work on
    int slice = arg.buffer_size / arg.children_number;

    // get and initialize buffer
    std::vector<int> buff(arg.buffer_size);
    init_rand(buff);

    // for every child
    std::vector<int> result_pipes(arg.children_number);
    for (int i = 0; i < result_pipes.size(); ++i) {
        // create two pipes, for parent to child
        // communication and vise versa
        int to_child_fd[2];
        int to_parent_fd[2];
        if (pipe(to_child_fd) == -1 || pipe(to_parent_fd) == -1) {
            throw std::runtime_error("error while creating a pipe");
        }

        // save the result pipe file descriptor
        result_pipes[i] = to_parent_fd[0];

        // get the range of a buffer and
        // give to child process throw pipe
        int range[2];
        range[0] = i * slice;
        range[1] = (i + 1) * slice;
        auto wrout = write(to_child_fd[1], range, sizeof(int) * 2);
        if (wrout == -1) {
            throw std::runtime_error("write failure");
        }

        // cloning the parent process
        int child_pid = fork();
        if (child_pid == -1) {
            throw std::runtime_error("error while using fork");
        }

        // if child process
        if (child_pid == 0) {
            // close unneeded pipes for reading and writing
            close(to_parent_fd[0]);
            close(to_child_fd[1]);

            // get results and exit
            child(buff, to_child_fd[0], to_parent_fd[1]);
            exit(0);
        } else {
            // the same way closing pipes for parent process
            close(to_parent_fd[1]);
            close(to_child_fd[0]);
        }
    }
    // print the results
    std::cout << "Total sum : " << get_total(result_pipes) << std::endl;
}