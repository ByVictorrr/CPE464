#ifndef CLIENT_H
#define CLIENT_H
#include "Utils.hpp"
#include <iostream>
#include <regex>
class TCPClient;
//typedef uint16_t hdr_t;

class UserParser{
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
        uint16_t formatMPacket(std::vector<std::string> &args, TCPClient &client);
        uint16_t formatBPacket(std::vector<std::string> &args, TCPClient &client);
        uint16_t PacketBuilder::formatLPacket(TCPClient &client);
        uint16_t PacketBuilder::formatEPacket(TCPClient &client);

    public:
        // For client to format stdinput returns size of bytes the full pdu is
        uint16_t formatPacket(const std::string &input, TCPClient &client);
          

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






#endif