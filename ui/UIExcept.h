//
// Created by dop on 2/14/21.
//

#ifndef PROJ1_UIEXCEPT_H
#define PROJ1_UIEXCEPT_H

#include <exception>
#include <string>

class UIExcept : public std::exception
{
public:
    UIExcept(const char* _msg) : msg {_msg}
    {}
    UIExcept(std::string _msg) : msg {_msg}
    {}
    std::string what() {return msg;}
private:
    std::string msg;
};

#endif //PROJ1_UIEXCEPT_H
