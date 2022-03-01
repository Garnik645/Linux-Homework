#include <iostream>
#include <csignal>
#include <cstdlib>
#include <ucontext.h>
#include <string>
#include <cerrno>
#include <cstring>
#include <pwd.h>

void handler(int signo, siginfo_t *info, void *vcontext)
{
    std::cout << "Signal Handler for signal N-" << signo << std::endl;
    std::cout << "-- PID of the sender : " << info->si_pid << std::endl;
    std::cout << "-- UID of the sender : " << info->si_uid << std::endl;

    struct passwd *pw = getpwuid(info->si_uid);
    if (pw != nullptr) {
        auto name = static_cast<std::string>(pw->pw_name);
        std::cout << "-- User name : " << name << std::endl;
    }

    auto* context = static_cast<ucontext_t*>(vcontext);
    auto mcontext = context->uc_mcontext;
    std::cout << "-- RIP register : " << mcontext.gregs[REG_RIP] << std::endl;
    std::cout << "-- RAX register : " << mcontext.gregs[REG_RAX] << std::endl;
    std::cout << "-- RBX register : " << mcontext.gregs[REG_RBX] << std::endl;
}

int main()
{
    struct sigaction act{};
    memset(&act, 0, sizeof(act));

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &handler;
    if (sigaction(SIGUSR1, &act, nullptr) == -1) {
        std::cerr << "sigaction fail with error " << errno << std::endl;
        exit(errno);
    }

    raise(SIGUSR1);
}
