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

#define INPUT_SIZE 255

void change_stream(std::string, const std::string&, int, mode_t);
void create_shell_folder();
bool get_command(char*, char**);
void exe_command(char**);