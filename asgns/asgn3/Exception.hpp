#ifndef EXECPTION_H_
#define EXECPTION_H_
#include <exception>
#include <iostream>
#include <string>
class CorruptPacketException : public std::exception{
    private:
        std::string msg;
    public:
        CorruptPacketException(const std::string& msg) : msg(msg){}
        inline virtual const char* what() const noexcept override
        {
            return msg.c_str();
        }
};

#endif