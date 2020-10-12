#include "Parsers.hpp"
/* ================CommandValidator================================ */

const int CommandValidator::MAX_INPUT = 1400;
const std::string CommandValidator::COMMAND_FORMATS = "^\\s*%("
                                                      "[M|m]\\s+([1-9]+)((\\s+[^\\s]+){1,})\\s*(.*)"
                                                      "|"
                                                      "[B|b]\\s*(.*)"
                                                      "|"
                                                      "[L|l]\\s*"
                                                      "|"
                                                      "[E|e]\\s*"
                                                      ")$";


const std::string MCommandValidator::FORMAT_MESSAGE ="Usage: %<M|m> num-handles(1-9) handle1 [handle2[..[handle9]]] [message]\n";

int MCommandValidator::findNumHandles(std::string &input){
    std::vector<std::string> args = splitByWhiteSpace(input); // maybe test args size
    if(args.size() >= 2)
        if(std::all_of(args[1].begin(), args[1].end(), ::isdigit))
            return std::stoi(args[1]);
    return -1; 

}
bool MCommandValidator::validate(std::string &input){
    int digitHandles;
    if((digitHandles = findNumHandles(input)) == -1){
        std::cout << FORMAT_MESSAGE;
        return false;
    }else if(digitHandles <= 0 || digitHandles > 9){
        std::cout << "Can only have digits of 1-9 handles" << std::endl;
        return false;
    }
    // if digitsHandles are valid
    std::vector<std::string> args = splitByWhiteSpace(input); // maybe test args size
    if((2+digitHandles) > (int)args.size()){
        std::cout << "Need digits (number of handles) to match arguments count for handles" << std::endl;
        return false;
    }
    // look at each handle make sure they are not greater than 100 
    for(int i = 2; i<2+digitHandles; i++){
        if(args[i].size() > 100){
            std::cout << "Each handle can only have at most 100 characters" << std::endl;
            return false;
        }
    }
    return true;
}


const std::string BCommandValidator::FORMAT_MESSAGE ="Usage: %<B|b> [message]\n";

bool BCommandValidator::validate(std::string &binput){
    std::regex bFormat("^\\s*%[B|b]\\s*(.*)$");
    std::smatch match;
    if (std::regex_search(binput, match, bFormat)){
        return true;
    }
    std::cout << FORMAT_MESSAGE;
    return false;
}
const std::string LCommandValidator::FORMAT_MESSAGE ="Usage: %<L|l>\n";

bool LCommandValidator::validate(std::string &linput){
    std::regex lFormat("^\\s*%[L|l]\\s*$");
    std::smatch match;
    if (std::regex_search(linput, match, lFormat)){
        return true;
    }
    std::cout << FORMAT_MESSAGE;
    return false;
}
const std::string ECommandValidator::FORMAT_MESSAGE ="Usage: %<E|e>\n";

bool ECommandValidator::validate(std::string &einput){
    std::regex EFormat("^\\s*%[E|e]\\s*$");
    std::smatch match;
    if (std::regex_search(einput, match, EFormat)){
        return true;
    }
    std::cout << FORMAT_MESSAGE;
    return false;
}



bool CommandValidator::validate(std::string &input){
            std::regex cmd_formats(COMMAND_FORMATS);
            // Step 1 - check to see if empty string
            if(input.empty()){
                return false;
            // Step 2 - is the input greater than MAX_INPUT 
            }else if(input.size() > MAX_INPUT){
                std::cout << "Cant exceed more than 1400 characters!" << std::endl;
                return false;
            }
                switch (std::toupper(CommandParser::getCommand(input)))
                {
                    case 'M':
                    {
                        return MCommandValidator::validate(input);
                    }
                        break;
                    case 'B':
                        return BCommandValidator::validate(input);
                        break;
                    case 'L':
                        return LCommandValidator::validate(input);
                        break;
                    case 'E':
                        return ECommandValidator::validate(input);
                        break;
                    default:
                        std::cout << "Usage: %<M|m> num-handles(1-9) handle1 [handle2[..[handle9]]] [message]\n"
                                "Usage: %<B|b> [message]\n"
                                "Usage: %<L|l>\n"
                                "Usage: %<E|e>" << std::endl;
                        return false;
                    break;
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

void MCommandParser::parse(std::string &input){
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
        std::cout << "MCommandParseer: Not able to parser\n";
    }
}

 void BCommandParser::parse(std::string &input){
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
                std::cout << "BCommandParser.parse: Not correct format\n";
            }
        }
