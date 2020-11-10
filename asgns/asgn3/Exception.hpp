#include <exception>
#include <iostream>
#include <string>
class CorruptPacketException : public std::exception{
    private:
        std::string msg;
    public:
        CorruptPacketException(const std::string& msg) : msg(msg){}
        virtual const char* what() const noexcept override
        {
            return msg.c_str();
        }
};
