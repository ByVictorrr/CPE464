#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H
#include "Utils.hpp"
#include <map>

class PacketParser{
    protected:
        uint16_t getPacketLen(uint8_t *pkt){
            uint16_t len;
            memcpy(&len, pkt, 2);
            return len;
        }
    public:
        virtual void parse(uint8_t *pkt) = 0;
};
/**
 *  For just the server
 */
class LoginPacketParser: public PacketParser{
    private:
        std::string handName;
    public:
        // Format: <hdr, 1byte handle lenght then handle no padding/nulls
        void parse(uint8_t *pkt){
            uint16_t len = getPacketLen(pkt);
            uint8_t flag = pkt[2];
            uint8_t handLen = pkt[3];
            this->handName = std::string(pkt+3, pkt+3+handLen);
        }
        std::string & getHandName(){return this->handName;}

};
/* Client and Server both use */
class BroadcastPacketParser: public PacketParser{
    private:
        std::string message;
        std::string srcHandle;
    public:
        void parse(uint8_t *pkt){
            // (1 byte containing the length of sending client | send client)/ then message
            // step 1 - get the size
            uint16_t len = getPacketLen(pkt);
            uint8_t handLen = pkt[3];
            this->message = std::string(pkt+3, pkt+3+handLen);
            this->srcHandle = std::string(pkt+3+handLen, pkt+len);
        }
        std::string &getMessage(){this->message;}
        std::string &getSourceHandle(){this->srcHandle;}

};

class MulticastPacketParser: public PacketParser{
    private:
        std::vector<std::string> destHandles;
        std::string message;
        std::string srcHandle;
        uint8_t numHandles;// only 1-9
        
    public:
    /* Expand to class for server and client to shar */
        void parse(uint8_t *pkt){
            // Step 1 - step 1 get src handle
            uint16_t pkt_len = getPacketLen(pkt);
            //uint8_t flag = 
            uint8_t srcHandleLen = pkt[3];
            this->srcHandle = std::string(pkt+3, (pkt+3)+srcHandleLen);
            // step 2 - get the message
            this->numHandles = pkt[srcHandle.size()+3];
            int start = srcHandle.size()+4;
            int i=start, numHan=0;
            while(i < pkt_len+start && numHan < numHandles){
                int hanLen = pkt[i];
                this->destHandles.push_back(std::string(pkt+i+1, pkt+i+1+hanLen));
                i+=(hanLen+1);
                numHan++;
            }
            // Got to the message (maybe check the )
            this->message = std::string((char*)(pkt+i));
        }
        std::vector<std::string> &getDestHandles(){this->destHandles;};
        std::string &getMessage(){this->message;}
        std::string &getSourceHandle(){this->srcHandle;}
        uint8_t &getNumHandles(){return this->numHandles;}
};

#endif