
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
        char cmd;
    public:
        static char getCommand(std::string &input){
           std::string tr = trim(input);
           return std::toupper(tr[1]);
        }
        virtual void parse(std::string &input) = 0;
};

class MCommandParser: public CommandParser{
    static const std::string COMMAND_FORMAT = "\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){0})(.*)$";
    private:
        /**
         * Variables of McommandParser
         */
        std::queue<std::string> messages;
        std::vector<std::string> destHandles;
        uint8_t numHandles;// only 1-9
       
    public:
        void parse(std::string &input)throw (const char *){
            int numHandles = getFirstDigit(input);
            const std::string format = "^\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){"
                                      +std::to_string(numHandles)+"})(.*)$";
            std::regex rgx(format);
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                // step 1 - get messages 
                std::string dests = trim_left(match[1]);
                this->destHandles = splitByWhiteSpace(dests);
                // step 2 - get dest handles
                std::string message = trim_left(match[3]);
                this->messages = split(message, 200);
            }
        }
        // Getters
        std::queue<std::string> &getMessages(){return this->messages;};
        uint8_t &getNumHandles(){return this->numHandles;}
        std::vector<std::string> &getDestHandles(){return this->destHandles;}

};
 /**
         * Helper functions for main parse function
         */
        uint8_t parseNumHandles(std::string &input)throw(const char *){
            uint8_t numHandles;
            std::regex rgx("\\s{0,}%[M|m]\\s{0,}([1-9])");
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                std::string handles = match[1];
                numHandles = std::stoi(handles.c_str());
                return numHandles;
            }
            throw "parseNumHandles: Couldn't find the number of handles";
        }
        std::vector<std::string> parseDestHandles(std::string &input, uint8_t handles) throw(const char*){
            std::string format = "\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){"+std::to_string(handles)+"})";
            std::vector<std::string> ret;
            std::regex rgx(format);
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                std::string handles = trim_left(match[1]);
                ret = splitByWhiteSpace(handles);
                return ret;
            }
        throw "parseDestHandles: The digit 1-9 has to match the number of handles given";
    }

    std::queue<std::string> parseMessages(std::string &input, uint8_t handles){

            std::string format = "\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){"+std::to_string(handles)+"})(.*)";
            std::queue<std::string> ret;
            std::regex rgx(format);

            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                std::string message = trim_left(match[3]);
                ret = split(message, 200);
                return ret;
            }
            // There is no message
            ret.push("\n");
        return ret;
    }
