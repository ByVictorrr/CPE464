#ifndef CLIENT_H
#define CLIENT_H
#include "Utils.hpp"
#include <iostream>
#include <regex>
#include <queue>
class TCPClient;
//typedef uint16_t hdr_t;


// usually used in the client to print out the message
class PacketBuilder{
        
    private:
        #define MAX_MESSAGE 200
       std::array<char, 1400-5> msg;
       std::string formatMPacket(std::vector<std::string> &args, TCPClient &client);
       std::string formatBPacket(std::vector<std::string> &args, TCPClient &client);
       std::string formatLPacket(TCPClient &client);
       std::string formatEPacket(TCPClient &client);

    public:
        // For client to format stdinput returns size of bytes the full pdu is
        std::string formatPacket(const std::string &input, TCPClient &client);
          

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
        uint16_t pkt_len;

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
        int send(uin16_t pkt_len){
            return safe_send(this->skt, this->transBuff, pkt_len, 0);
        }
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