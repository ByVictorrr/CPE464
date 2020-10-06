#ifndef NETWORK_NODES_H
#define NETWORK_NODES_H
#include <iostream>
#include <sys/socket.h>
#include "safe_sys_calls.h"
#include <netinet/in.h>
#include "gethostbyname6.h"
#include <vector>
#include <sstream>
#include <regex>

#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10

int selectCall(int socketNumber, int seconds, int microseconds, int timeIsNotNull);



class CommandValidator{
    private:
    public:
        // Assumed that input is trimed
        static bool validateInput(std::string &input){
            static const char *COMMAND_FORMAT = "%([M|m]\\s{1,}[1-9]\\s{1,}[^\\s]{1,100}(:?\\s{1,}[^\\s]{1,100}){0,8}?(:?\\s{1,}[^\\s]{1,}){0,}?|[B|b](\\s{1,}[^\\s]{0,}){0,}?|[L|l]\\s{0,}|[E|e]\\s{0,}?)?$";
            std::regex cmd_format(COMMAND_FORMAT);
            char cmd = input[1];
            // Step 1 - see if input matches one of the commands format
            if(!std::regex_match(input, cmd_format)){
                switch (std::toupper(cmd))
                {
                case 'M':
                    throw "Usage: %<M|m> num-handles(1-9) handle1 [handle2[..[handle9]]] [message]";
                case 'B':
                    throw "Usage: %<B|b> [message]";
                case 'L':
                    throw "Usage: %<L|l>";
                case 'E':
                    throw "Usage: %<E|e>";
                default:
                    throw "Usage: %<M|m> num-handles(1-9) handle1 [handle2[..[handle9]]] [message]\n \
                           Usage: %<B|b> [message]\n \
                           Usage: %<L|l>\n \
                           Usage: %<E|e>";
                }
            }
            // step 2 - matches one of the commands
            return true;
        }
            
        static std::vector<std::string> parse(std::string &str){
            // Also sets it
            std::vector<std::string> result; 
            std::istringstream iss(str); 
            for(std::string s; iss >> s; ) 
                result.push_back(s); 
            return result;
        }
        //
        //Left trim
        //
        static std::string trim_left(const std::string& str)
        {
        const std::string pattern = " \f\n\r\t\v";
        return str.substr(str.find_first_not_of(pattern));
        }

        //
        //Right trim
        //
        static std::string trim_right(const std::string& str)
        {
        const std::string pattern = " \f\n\r\t\v";
        return str.substr(0,str.find_last_not_of(pattern) + 1);
        }

        //
        //Left and Right trim
        //
        static std::string trim(const std::string& str)
        {
        return trim_left(trim_right(str));
        }

};

#endif
