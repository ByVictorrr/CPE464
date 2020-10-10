#include "Parsers.hpp"
/* ================CommandValidator================================ */

const int CommandValidator::MAX_INPUT = 1400;
const std::string CommandValidator::COMMAND_FORMATS = "^\\s{0,}%("
                                                      "[M|m]\\s{1,}[1-9](\\s{1,}[^\\s]{1,100}){1,9}?(\\s{1,}[^\\s]{1,}){0,}"
                                                      "|"
                                                      "[B|b](\\s{1,}[^\\s]{1,}){0,}"
                                                      "|"
                                                      "[L|l]\\s{0,}"
                                                      "|"
                                                      "[E|e]\\s{0,}"
                                                      ")\\s{0,}$";
bool CommandValidator::validate(std::string &input){
            std::regex cmd_formats(COMMAND_FORMATS);
            char cmd = CommandParser::getCommand(input);
            if(input.size() > MAX_INPUT){
                std::cout << "Cant exceed more than 1400 characters!" << std::endl;
                return false;
            }
            else if(!std::regex_match(input, cmd_formats)){
                switch (std::toupper(cmd))
                {
                case 'M':
                    std::cout << "Usage: %<M|m> num-handles(1-9) handle1 [handle2[..[handle9]]] [message]" << std::endl;
                case 'B':
                    std::cout << "Usage: %<B|b> [message]" << std::endl;
                case 'L':
                    std::cout << "Usage: %<L|l>" << std::endl;
                case 'E':
                    std::cout << "Usage: %<E|e>" << std::endl;
                default:
                    std::cout << "Usage: %<M|m> num-handles(1-9) handle1 [handle2[..[handle9]]] [message]\n"
                           "Usage: %<B|b> [message]\n"
                           "Usage: %<L|l>\n"
                           "Usage: %<E|e>" << std::endl;
                }
                return false;
            }
            return true;
        }

/* ============================================================== */
                    
/* ======================CommandParser============================*/
char CommandParser::getCommand(std::string &input){
        std::string tr = trim(input);
        return std::toupper(tr[1]);
}

void MCommandParser::parse(std::string &input)throw (const char *){
    this->numHandles = getFirstDigit(input);
    this->cmd = CommandParser::getCommand(input);
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

 void BCommandParser::parse(std::string &input)throw (const char *){
            this->cmd = CommandParser::getCommand(input);
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
