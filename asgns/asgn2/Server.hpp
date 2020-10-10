#ifndef SERVER_H
#define SERVER_H

#include "Utils.hpp"
#include <map>
#include "safe_sys_calls.h"

class Server{
    protected:
        int skt; 
        std::map<int, std::string> clients;
        int port; // maybe remove
        uint8_t recvBuff[MAX_BUFF];
        uint8_t transBuff[MAX_BUFF];

    public:
        Server(int port, int type, int protocol=0) ;
        ~Server(){}
        virtual void config() = 0;
        bool inClients(std::string &handle){
            for(const auto & p: this->clients)
                if(p.second.compare(handle))
                    return true;
            return false;
        }

};



class TCPServer: public Server{

    void processClient(int fd);
    public:
        TCPServer(int port, int protocol=0);
        void config();
        int acceptClient(int server_socket, int debugFlag);
        void loop();

};


class PacketParser{
    public:
        //virtual void parse(){}
};
/**
 *  For just the server
 */
class LoginPacketParser{
    private:
        std::string handName;
    public:
        // Format: <hdr, 1byte handle lenght then handle no padding/nulls
        void parse(uint16_t data_len, uint8_t *data){
            uint8_t handLen = data[0];
            this->handName = std::string(data+1, data+1+handLen);
        }
        std::string & getHandName(){return this->handName;}

};
/* Client and Server both use */
class BroadcastPacketParser{
    private:
        std::string message;
    public:
        void parse(uint8_t *buff){
            uin
            // (1 byte containing the length of sending client | send client)/ then message
            // step 1 - get the size
            uint8_t handLen = data[0];
            std::string srcHand = std::string(data+1, data+1+handLen);

        }

};

class MulticastPacketParser{
    private:
        std::vector<std::string> destHandles;
        std::string message;
    public:
    /* Expand to class for server and client to shar */
        void parse(uint16_t data_len, uint8_t *data){
            // Step 1 - step 1 get src handle
            uint8_t srcHandleLen = data[0];
            std::string &&srcHandle = std::string(data+1, (data+1)+srcHandleLen);
            // step 2 - get the message
            uint8_t numHandles = data[srcHandle.size()+1];
            int start = srcHandle.size()+2;
            int i=start, numHan=0;
            while(i < data_len+start && numHan < numHandles){
                int hanLen = data[i];
                this->destHandles.push_back(std::string(data+i+1, data+i+1+hanLen));
                i+=(hanLen+1);
                numHan++;
            }
            // Got to the message (maybe check the )
            this->message = std::string((char*)(data+i));
        }
        std::vector<std::string> &getDestHandles(){this->destHandles};
        std::string &getMessage(){this->message;}

};


#endif