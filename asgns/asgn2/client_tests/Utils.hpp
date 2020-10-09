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
#include "Client.hpp"
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

class PacketParser{
    private:
     
    public:
        static bool parse(uint8_t flag, uint16_t data_len,uint8_t *data) throw (const char *){
        switch(flag){
            // handle is good (dont worry about parsing)
            case HANDLE_DNE:
                std::cout << "Login sucess!" << std::endl;
                return true;
                break;
            // handle already exists (exit)
            case HANDLE_EXISTS:
                std::cerr << "Handle already exists; try a differnt one" << std::endl;
                return false;
                break;
            case BROADCAST:
                {
                    // b
                }
                break;
            case MULTICAST:
            {
                return MultiCastParse(data_len, data);
            }
               break;
            case MESSAGE_HANDLE_DNE:
            {
                // when the destination handle for %m doesnt exist 
                parser.parse(data)
                std::string dneHandle = parser.getHandle();
                std::cerr << "Handle " + dneHandle +"" << std::endl;
            }
                break;
            case CLIENT_EXIT_ACK:
                this->close();
                exit(EXIT_SUCCESS);
                break;
            case NUM_HANDLES:
            {
                NumberOfHandlePacketParser parser; 
                parser.parse(data);
                this->numHandles = parser.getNumHandles(); 
                // server gives a packet with number of all handles on the server
            }
                break;
            case LIST_HANDLE_NAME:
            {

            }
            break;
            case FINISHED_LIST_HANDLES:
            {

            }
            break;
                
            }        
            return true;
        }

}



#endif
