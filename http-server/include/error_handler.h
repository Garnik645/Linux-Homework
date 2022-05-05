#ifndef A_OUT_ERROR_HANDLER_H
#define A_OUT_ERROR_HANDLER_H

#include <iostream>
#include <exception>
#include <string>
#include <cerrno>

template<typename T>
void error_handler(const T &returnValue, const T &errorValue, const std::string &errorMessage) {
    if (returnValue == errorValue) {
        throw std::runtime_error(errorMessage + " Error : " + std::to_string(errno));
    }
}

#endif //A_OUT_ERROR_HANDLER_H
