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
#include <Client.hpp>

#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10
#define TIME_IS_NOT_NULL 0

int selectCall(int socketNumber, int seconds, int microseconds, int timeIsNotNull);

typedef enum FLAGS{
    NOT_USED,
    CHECK_HANDLE,
    HANDLE_DNE,
    HANDLE_EXISTS,
    BROADCAST,
    MESSAGE,
    NOT_USED,
    MESSAGE_HANDLE_DNE,
    CLIENT_EXIT,
    CLIENT_EXIT_ACK,
    LIST_HANDLES,
    NUM_HANDLES,
    LIST_HANDLE_NAME,
    FINISHED_LIST_HANDLES
} flags_t;


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
        std::vector<std::string> &splitWhitespace(std::string &input){
            std::vector<std::string> result; 
            std::istringstream iss(input); 
            for(std::string s; iss >> input; ) 
                result.push_back(s); 
            return result;
        }
        /**
         * Assume input[0] = % and input[1] = [M|m]
         * /* input
         */
        int fomatMPacket(std::string args, TCPClient &client){
            
            int num_handles;
            int cursor=0;
            /* args format: <num-dests> <dest-handle1> ... [dest-handlen] [text] */
            std::vector<std::string> &parsed = splitWhitespace(args);

            std::vector<uint8_t> handle_lens;
            std::vector<std::string&> handles;
            std::string &msg;

            // Step 1 - check to see if num-handles is there 
            if(!(num_handles = std::atoi(parsed[0].c_str())) || num_handles < 0 || num_handles > 9){
                std::cerr << "Usage: %[m|M] num-handles(1-9) dest-handle1 .. [dest-handle9] [text]" << std::endl;
            }
            // Step 2 - find the handles (assumed to be inex [1,size-2])
            for (size_t i = 1; i < num_handles+1; i++){
                handles[i-1]  = parsed[i];
                handle_lens[i-1] = handles[i-1].length();
            }

            // Step 3 - get msg pased[size-1] 
            // Case to see if a msg exists
            if(num_handles+1 < parsed.size()){
                msg = parsed[parsed.size()-1];
            }else{
                msg = "\n";
            }
            char *src_handle = client.handle;
            uint8_t *transBuff = client.transBuff;
            uint16_t pkt_len = HDR_LEN+FLAG_LEN;

            for(uint8_t handle_len: handle_lens){
                pkt_len += (handle_len+1);
            }
            pkt_len+=msg.size()+1; // plus one for the null

            // Done with the length
            transBuff[cursor++] = pkt_len >> 8;
            transBuff[cursor++] = pkt_len;
            transBuff[cursor++] = MESSAGE;
            transBuff[cursor++] = strlen(client.handle);
            // Step 2 - get the handles in the field s
            memcpy(transBuff+cursor, client.handle, strlen(client.handle));
            cursor+= strlen(client.handle);
            // Do the destination handles
            for(int i =0; i< num_handles; i++){
               transBuff[cursor++] = num_handles[i];
               memcpy(transBuff+cursor, handles[i], handles[i].size());
               cursor+=handles[i].size();
            }

            // DO the msg
            memcpy(transBuff+cursor, msg, msg.size()+1);


        }


    public:
        // For client to format stdinput
        uint8_t *formatPacket(char *input, uint16_t len, TCPClient &client){
            uint8_t flag;
            uint16_t pkt_len = len;
            std::string parsed_input = std::regex_replace(input, std::regex("^ +| +$|( ) +"), "$1");
            char cmd = parsed_input[1];

            // Case 2 - stdin has a command
            if( len < 2 || input[0] != '%' || !inCommnds(input[1])){
                std::cout << "Not a valid command" << std::endl;
                return NULL;
            }
            // Step 1 - parse to get all the fields then get size
            switch (toupper(parsed_input[1]))
            {
            case 'M':
                fillMComand(parsed_input[2], client);
                break;
            
            default:
                break;
            }


            // Step 3 - set all the fields
            input[0] = (uint8_t)(len >> 8);
            input[1] = (uint8_t)len;
            


	    }
};



#endif
