#include "Parsers.hpp"
/* ================CommandValidator================================ */

const int CommandValidator::MAX_INPUT = 1400;
const std::string CommandValidator::COMMAND_FORMATS = "^\\s*%("
                                                      "[M|m]\\s+[1-9]((\\s+[^\\s]+){1,9})\\s*(.*)"
                                                      "|"
                                                      "[B|b]\\s*(.*)"
                                                      "|"
                                                      "[L|l]\\s*"
                                                      "|"
                                                      "[E|e]\\s*"
                                                      ")$";
bool CommandValidator::validate(std::string &input){
            std::regex cmd_formats(COMMAND_FORMATS);
            if(input.empty()){
                return false;
            }else if(input.size() > MAX_INPUT){
                std::cout << "Cant exceed more than 1400 characters!" << std::endl;
                return false;
            }
            else if(!std::regex_match(input, cmd_formats)){
                switch (std::toupper(CommandParser::getCommand(input)))
                {
                case 'M':
                    MCommandValidator::validate(input);
                    break;
                case 'B':
                    break;
                case 'L':
                    break;
                case 'E':
                    break;
                default:
                    std::cout << "Usage: %<M|m> num-handles(1-9) handle1 [handle2[..[handle9]]] [message]\n"
                           "Usage: %<B|b> [message]\n"
                           "Usage: %<L|l>\n"
                           "Usage: %<E|e>" << std::endl;
                break;
                }
                return false;
            }
            return true;
        }

/* ============================================================== */
                    
/* ======================CommandParser============================*/
// Assumed correct input
char CommandParser::getCommand(std::string &input){
        std::string tr = trim(input);
        return std::toupper(tr[1]);
}

void MCommandParser::parse(std::string &input)throw (const char *){
    this->numHandles = getFirstDigit(input);
    this->cmd = CommandParser::getCommand(input);
    const std::string format = "^\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){"
                                +std::to_string(this->numHandles)+"})\\s*(.*)$";
    std::regex rgx(format);
    std::smatch match;
    if (std::regex_search(input, match, rgx)){
        // step 1 - get messages 
        std::string dests = trim(match[1]); // may need to trim because captures a group with a space infront
        this->destHandles = splitByWhiteSpace(dests);
        // step 2 - get dest handles
        if(!match[3].length()){
            this->messages.push("\n");
        }else{
            std::string message = trim(match[3]); // may need to trim right
            this->messages = split(message, 200);
        }
    }else{
        throw "(1)Can only 1-100 characters/handle\n(2)Number of handles need to match the number of arguments following";
    }
}

 void BCommandParser::parse(std::string &input)throw (const char *){
            this->cmd = CommandParser::getCommand(input);
            const std::string format = "^\\s*%[B|b]\\s*(.*)$";
            std::regex rgx(format);
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                if(!match[1].length()){
                    this->messages.push("\n");
                }else{
                    std::string message = trim(match[1].str()); // No need to split due to the regex (if no space then it work)
                    this->messages = split(message, 200);
                }
            }else{
                throw "BCommandParser.parse: Not correct format(should never get here)";
            }
        }
