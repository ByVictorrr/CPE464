#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <sys/socket.h>
#include "safe_sys_calls.h"
#include <netinet/in.h>
#include "gethostbyname6.h"
#include <vector>
#include <sstream>
#include <regex>
#include <string>
#include <queue>
#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10


typedef enum FLAGS{
    NOT_USED,
    CHECK_HANDLE,
    HANDLE_DNE,
    HANDLE_EXISTS,
    BROADCAST,
    MULTICAST,
    UN_USED,
    MESSAGE_HANDLE_DNE,
    CLIENT_EXIT,
    CLIENT_EXIT_ACK,
    LIST_HANDLES,
    NUM_HANDLES,
    LIST_HANDLE_NAME,
    FINISHED_LIST_HANDLES
} flags_t;
 

class CommandValidator{
    private:
    public:
        // Assumed that input is trimed
        static bool validateInput(std::string &input){
            static const char *COMMAND_FORMAT = "^\\s{0,}%([M|m]\\s{1,}[1-9](\\s{1,}[^\\s]{1,100}){1,9}?(\\s{1,}[^\\s]{1,}){0,}|[B|b](\\s{1,}[^\\s]{1,}){0,}|[L|l]\\s{0,}|[E|e]\\s{0,})\\s{0,}$";
            std::regex cmd_format(COMMAND_FORMAT);
            char cmd = input[1];
            // Step 1 - check to see if input size is greater than max stdin
            if(input.size() > 1400){
                throw "Cant exceed more than 1400 characters!";
            }
            // Step 2 - see if input matches one of the commands format
            else if(!std::regex_match(input, cmd_format)){
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
};

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

        std::string trim_left(const std::string& str){
            const std::string pattern = " \f\n\r\t\v";
            return str.substr(str.find_first_not_of(pattern));
        }
        std::string trim_right(const std::string& str){
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
        std::string trim(const std::string& str){
            return trim_left(trim_right(str));
         }
        char getCommand(std::string &input){
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
        uint8_t numHandles;// only 1-9
        std::vector<std::string> destHandles;
        /**
         * Helper functions for main parse function
         */
        uint8_t parseNumHandles(std::string &input)throw(const char *){
            uint8_t numHandles;
            std::regex rgx("\\s{0,}%M\\s{0,}([1-9])");
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
        
        std::queue<std::string> getMessages(){return this->messages;};
        uint8_t getNumHandles(){return this->numHandles;}
        std::vector<std::string> getDestHandles(){return this->destHandles;}

        friend class PacketFactory;
};

class PacketFactory{

    public:
        static uint16_t getPacketLen(uint8_t*pkt){
            uint16_t len;
            memcpy(&len, pkt, 2);
            return len;
        }
        /* Format <hdr len (2) | 1 byte flag | 1 byte src-han-len | src-han | dest-han1-len | dest-han1 | ... | msg */
        static uint8_t *buildMPacket(MCommandParser &cmd, TCPClient &client){
            uint8_t flag = MULTICAST;
            uint8_t *transBuff = client.getTransBuff();
            char *srcHand = client.getHandle();
            uint8_t src_handle_len = std::strlen(srcHand);
            int cursor = 0;
            /** Get pkt_size first */
            uint16_t pkt_len = HDR_LEN+FLAG_LEN;
            // Step 1 - src-handle length
            pkt_len+=(src_handle_len+1);
            // Step 2- - all destinations-handles length 
            for(int i = 0; i < cmd.destHandles.size(); i++){
                pkt_len+=(1+cmd.destHandles[i].size());
            }
            pkt_len = cmd.messages.front().size()+1; // +1 for including the null char
            /** Done getting the pkt_size */

            /** Time to fill the trans buff */
            memcpy(transBuff, &pkt_len, 2);
            cursor=2;
            memcpy(transBuff+cursor, &flag, 1);
            cursor+=1;
            transBuff[cursor] = src_handle_len;
            cursor+=1;
            memcpy(transBuff+cursor, srcHand, src_handle_len);
            cursor+=src_handle_len;
            for(std::string &destHand: cmd.destHandles){
                transBuff[cursor++] = destHand.size();
                memcpy(transBuff+cursor, destHand.c_str(), destHand.size());
                cursor+=destHand.size();
            }
            // set message
            memcpy(transBuff+cursor, cmd.messages.front().c_str(), cmd.messages.front().size());
            return transBuff;
        }
};







#endif
