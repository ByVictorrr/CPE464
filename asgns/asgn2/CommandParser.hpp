
#include <iostream>
#include <vector>
#include <sstream>
#include <queue>
#include <regex>
#include "Utils.hpp"


class CommandValidator{
    private:
        static const int MAX_INPUT;
        static const std::string COMMAND_FORMATS;
    public:
        static bool validate(std::string &input);
};

class CommandParser{
    protected:
        char flag;
    public:
        static char getCommand(std::string &input){
           std::string tr = trim(input);
           return std::toupper(tr[1]);
        }
        virtual void parse(std::string &input) = 0;
};

class MCommandParser: public CommandParser{
    private:
        std::queue<std::string> messages;
        std::vector<std::string> destHandles;
        uint8_t numHandles;// only 1-9
    public:
        void parse(std::string &input)throw (const char *){
            this->numHandles = getFirstDigit(input);
            this->flag = getCommand(input);
            const std::string format = "^\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){"
                                      +std::to_string(this->numHandles)+"})(.*)$";
            std::regex rgx(format);
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                // step 1 - get messages 
                std::string dests = trim_left(match[1]);
                this->destHandles = splitByWhiteSpace(dests);
                // step 2 - get dest handles
                std::string message = trim_left(match[3]);
                this->messages = split(message, 200);
            }else{
                throw "MCommandParser.parse: not able to parse";
            }
        }
        // Getters
        std::queue<std::string> &getMessages(){return this->messages;};
        uint8_t &getNumHandles(){return this->numHandles;}
        std::vector<std::string> &getDestHandles(){return this->destHandles;}
};

class BCommandParser: public CommandParser{
    private:
        std::queue<std::string> messages;
    public:
        void parse(std::string &input)throw (const char *){
            int numHandles = getFirstDigit(input);
            const std::string format = "^\\s*%[B|b]\\s+(.*)$";
            std::regex rgx(format);
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                // step 1 - get messages 
                std::string message = trim_left(match[1]);
                this->messages = split(message, 200);
            }else{
                throw "BCommandParser.parse: not able to parse";
            }
        }
        // Getters
        std::queue<std::string> &getMessages(){return this->messages;};
};


class LCommandParser: public CommandParser{
    private:

    public:


};

