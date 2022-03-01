#include <iostream>
#include <csignal>
#include <cstdlib>
#include <ucontext.h>
#include <string>
#include <cerrno>
#include <cstring>
#include <pwd.h>

// function for handling SIGUSR1
void handler(int signo, siginfo_t *info, void *vcontext) {
    // print info
    std::cout << "Signal Handler for signal N-" << signo << std::endl;
    std::cout << "-- PID of the sender : " << info->si_pid << std::endl;
    std::cout << "-- UID of the sender : " << info->si_uid << std::endl;

    // get username with error handling
    struct passwd *pw = getpwuid(info->si_uid);
    if (pw != nullptr) {
        auto name = static_cast<std::string>(pw->pw_name);
        std::cout << "-- User name : " << name << std::endl;
    }

    // print register state
    auto *context = static_cast<ucontext_t *>(vcontext);
    auto mcontext = context->uc_mcontext;
    std::cout << "-- RIP register : " << mcontext.gregs[REG_RIP] << std::endl;
    std::cout << "-- RAX register : " << mcontext.gregs[REG_RAX] << std::endl;
    std::cout << "-- RBX register : " << mcontext.gregs[REG_RBX] << std::endl;
}

int main() {
    // action for signal SIGUSR1
    struct sigaction act{};

    // fill all the bytes with 0s in sigaction structure
    memset(&act, 0, sizeof(act));

    // set SA_SIGINFO flag to use context and info in handler
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &handler;

    // run sigaction with error handling for SIGUSR1
    if (sigaction(SIGUSR1, &act, nullptr) == -1) {
        std::cerr << "sigaction fail with error " << errno << std::endl;
        exit(errno);
    }

    // raise signal SIGUSR1 to test signal handler
    raise(SIGUSR1);
}
