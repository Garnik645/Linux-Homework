#pragma once
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

void change_stream(std::string dir, const std::string& filename, int stream, mode_t mode);
void create_shell_folder();
bool get_command(char** arg);
void call_command(char** arg);