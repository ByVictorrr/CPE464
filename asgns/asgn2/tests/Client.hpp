#ifndef CLIENT_H
#define CLIENT_H
#include "Utils.hpp"
#include <iostream>
#include <regex>
class TCPClient;

class UserParser{
    private:

        const char *M_REGEX = "^\s{0,}%M\s{1,}[1-9]\s{1,}[^\s]{1,100}(:?\s{1,}[^\s]{1,100}){0,9}?$";
        const char *B_REGEX = "^\s{0,}%B\s{0,}[^\s]{1,}";

       public:
            // Assumed that input is trimed
            static bool validateInput(std::string &input){
                static const char *COMMAND_FORMAT = "%(M\s{1,}[1-9]\s{1,}[^\s]{1,100}(:?\s{1,}[^\s]{1,100}){0,8}?(:?\s{1,}[^\s]{1,}){0,}?|B(\s{1,}[^\s]{0,}){0,}?|L\s{0,}|E\s{0,}?)?$";
                std::regex cmd_format(COMMAND_FORMAT);
                char cmd = input[1];
                // Step 1 - see if input matches one of the commands format
                if(!std::regex_match(input, cmd_format)){
                    switch (std::toupper(cmd))
                    {
                    case 'M':
                        std::cout << "usage: %[M|m] num-handles(1-9) handle1 [handle2[..[handle9]]] [message]" << std::endl;
                        return false;
                        break;
                    
                    default:
                        std::cout << "command " << cmd << " Not found" << std::endl;
                        return false;
                        break;
                    }
                }
                // step 2 - matches one of the commands
                return true;
            }
    
            static std::vector<std::string> parse(std::string &validated){
                // Also sets it
                std::vector<std::string> result; 
                std::istringstream iss(validated); 
                for(std::string s; iss >> s; ) 
                    result.push_back(s); 
                return result;
            }

};

// usually used in the client to print out the message
class PacketBuilder{
    protected:
        static const int NUM_COMMANDS = 4;
	    const char COMMANDS[NUM_COMMANDS] = {
            'M',
            'B',
            'L',
            'E',
        };
        bool inCommnds(char c){
            for(int i=0; i <NUM_COMMANDS; i++){
                if(toupper(c) == COMMANDS[i]){
                    return true;
                }
            }
            return false;
        }
        /**
         * Assume input[0] = % and input[1] = [M|m]
         * 
         */
        uint16_t formatMPacket(std::string &args, TCPClient &client);


    public:
        // For client to format stdinput
        uint16_t formatPacket(std::string &input, TCPClient &client);
          

};
class Client{
    protected:
        /* Data that is needed as client */
        int skt; 
        char *port;
        char *handle;
        char *serverName;

        /* Chat-protocol Buffers */
        uint8_t recvBuff[MAX_BUFF];
        uint8_t transBuff[MAX_BUFF];

        /* Inputs of a client */
        std::string readUserInput(); // helper of recv
        size_t processSocket(); // helper of recv

        /* How to parse/build messages to be sent or recv*/

    public:
        Client(char *handle, char *server_name, char *port, int type, int protocol=0);
        ~Client();
        void close();
};

class TCPClient: public Client{
    private:
        PacketBuilder pb;
    public:
        TCPClient(char *handle, char *server_name, char *port, int protocol=0);
        void connect(int debugFlag);
        // like main
        void loop();
    
        friend class PacketBuilder;
};


typedef enum FLAGS{
    NOT_USED,
    CHECK_HANDLE,
    HANDLE_DNE,
    HANDLE_EXISTS,
    BROADCAST,
    MESSAGE,
    UN_USED,
    MESSAGE_HANDLE_DNE,
    CLIENT_EXIT,
    CLIENT_EXIT_ACK,
    LIST_HANDLES,
    NUM_HANDLES,
    LIST_HANDLE_NAME,
    FINISHED_LIST_HANDLES
} flags_t;






#endif