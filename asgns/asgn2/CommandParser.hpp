
#include <iostream>
#include <vector>
#include <sstream>
#include <queue>
#include <regex>

class CommandParser{
    protected:
        char cmd;
        static std::vector<std::string> splitByWhiteSpace(std::string &input){
            std::vector<std::string> result; 
            std::istringstream iss(input); 
            for(std::string s; iss >> s; ) 
                result.push_back(s); 
            return result;
        }

        static std::string trim_left(const std::string& str){
            const std::string pattern = " \f\n\r\t\v";
            return str.substr(str.find_first_not_of(pattern));
        }
        static std::string trim_right(const std::string& str){
            const std::string pattern = " \f\n\r\t\v";
            return str.substr(0,str.find_last_not_of(pattern) + 1);
        }
        static std::queue<std::string> split(const std::string str, int splitLen){
            std::queue<std::string> ret;
            for (size_t i = 0; i < str.size(); i += splitLen)
                ret.push(str.substr(i, splitLen));
            return ret;
        }
    public:
         static std::string trim(const std::string& str){
            return trim_left(trim_right(str));
         }
        static char getCommand(std::string &input){
           std::string tr = trim(input);
           return std::toupper(tr[1]);
        }
        virtual void parse(std::string &input) = 0;
};

class MCommandParser: public CommandParser{

    private:
        /**
         * Variables of McommandParser
         */
        std::queue<std::string> messages;
        std::vector<std::string> destHandles;
        uint8_t numHandles;// only 1-9
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

    public:

        // main function
        void parse(std::string &input)throw (const char *){
            this->numHandles = parseNumHandles(input);
            this->destHandles = parseDestHandles(input, this->numHandles);
            this->messages = parseMessages(input, this->numHandles);
        }
        // Getters
        
        std::queue<std::string> &getMessages(){return this->messages;};
        uint8_t &getNumHandles(){return this->numHandles;}
        std::vector<std::string> &getDestHandles(){return this->destHandles;}

        friend class PacketFactory;
};
