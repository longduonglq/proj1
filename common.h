//
// Created by dop on 2/13/21.
//

#ifndef PROJ1_COMMON_H
#define PROJ1_COMMON_H

#include <iostream>
#include <exception>
#include <stdlib.h>

#ifdef _WIN32
void clearScreen() {
    system("cls");
}
#elif __APPLE__

void clearScreen() {
    std::cout << std::string(40, '\n');
    // system("clear");
}

#else
void clearScreen() {
    std::cout << "Unknown OS" << std::endl;
    std::exit(1);
}
#endif


class ldException : public std::exception
{
public:
    ldException(const char* _msg)
        :
        msg {_msg}
    {}

    const char* what() {return msg.c_str();}
private:
    std::string msg;
};

#endif //PROJ1_COMMON_H
