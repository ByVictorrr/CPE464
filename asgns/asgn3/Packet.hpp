#ifndef PACKET_H_
#define PACKET_H_
#include <iostream>
#include <utility>
#include "Exception.hpp"

#define HDR_LEN 7
#define MAX_PAYLOAD_LEN 1400
#define MAX_FILENAME_LEN 100
#define MAX_SETUP_PAYLOAD_LEN 108

typedef enum FLAGS{
    NOT_USED,
    OPTIONAL1,
    OPTIONAL2,
    DATA_PACKET,
    NOT_USED2,
    RR_PACKET,
    SREJ_PACKET,
    FILENAME_PACKET,
    FILENAME_PACKET_OK,
    FILENAME_PACKET_BAD,
    EOF_PACKET,
    EOF_PACKET_ACK
}flag_t;

class RCopyHeader{
    protected:
        uint32_t sequenceNum;
        uint16_t checksum;
        uint8_t flag;
    public:
        inline RCopyHeader(uint32_t seqNum, uint16_t checksum, uint8_t flag): sequenceNum(seqNum), checksum(checksum), flag(flag){}
        inline RCopyHeader(){}
        /* Settters */
        inline void setSequenceNumber(uint32_t seqNum){ this->sequenceNum = seqNum;}
        inline void setChecksum(uint16_t checksum){this->checksum = checksum;}
        inline void setFlag(uint8_t flag){this->flag = flag;}
        /* Getters */
        inline uint32_t &getSequenceNumber(){return this->sequenceNum;}
        inline uint16_t &getChecksum(){return this->checksum;}
        inline uint8_t &getFlag(){return this->flag;}
        inline int getLen(){
            return sizeof(sequenceNum)+sizeof(checksum)+sizeof(flag);
        }

        /* Utility functions */
        inline void clear(){ 
            this->sequenceNum = 0;
            this->checksum = 0;
            this->flag = 0;
        }
};


class RCopyPacket{
    protected:
        RCopyHeader header;
        uint8_t payload[MAX_PAYLOAD_LEN];
        int payloadSize;
        // Assume that header and payload is set
        inline uint16_t computeChecksum(RCopyHeader &header, uint8_t *payload, int payloadSize){
            uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
            uint16_t cksum;
            int cursor;
            memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
            // step 1 - packet
            memcpy(temp, &header.getSequenceNumber(), cursor=sizeof(header.getSequenceNumber()));
            memcpy(temp+cursor, &header.getChecksum(), sizeof(header.getChecksum()));
            cursor+=sizeof(header.getChecksum());
            memcpy(temp+cursor, &header.getFlag(), sizeof(header.getFlag()));
            cursor+=sizeof(header.getFlag());
            memcpy(temp+cursor, payload, payloadSize);
            cksum = in_cksum((unsigned short *)temp, payloadSize+HDR_LEN);
            return cksum;
        }
    public:
        RCopyPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadSize) 
        : header(seqNum, 0, flag), payloadSize(payloadSize){
            memset(this->payload, 0, MAX_PAYLOAD_LEN);
            if(payload != NULL)
                memcpy(this->payload, payload, payloadSize);
            header.setChecksum(computeChecksum(header, payload, payloadSize));
        }
        RCopyPacket(){}

        inline void clear(){
            memset(payload, 0, MAX_PAYLOAD_LEN);
            header.clear();
        }
        inline RCopyHeader &getHeader(){
            return this->header;
        }
        inline uint8_t *getPayload(){return this->payload;}
        inline uint8_t *getRawPacket(){
            uint16_t checksum;
            static uint8_t rawPacket[MAX_PAYLOAD_LEN+HDR_LEN];
            memset(rawPacket, 0, MAX_PAYLOAD_LEN+HDR_LEN);
            memcpy(rawPacket, &header.getSequenceNumber(), sizeof(uint32_t));
            memcpy(rawPacket+sizeof(uint32_t), &header.getChecksum(), sizeof(uint16_t));
            memcpy(rawPacket+sizeof(uint32_t)+sizeof(uint16_t), &header.getFlag(), sizeof(uint8_t));
            memcpy(rawPacket+sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint8_t), payload, payloadSize);
            return rawPacket;
        }
        inline int getPayloadLen(){
            return this->payloadSize;
        }
        inline int getPacketLen(){
            return this->payloadSize+header.getLen();
        }


};

class RCopySetupPacket: public RCopyPacket{
    protected:
        uint32_t bufferSize;
        uint32_t windowSize;
        char *fileName;
    public:
        RCopySetupPacket(uint32_t bufferSize, uint32_t windowSize, char *filename, uint8_t* payload)
        : RCopyPacket(0, FILENAME_PACKET, payload, MAX_SETUP_PAYLOAD_LEN), 
          bufferSize(bufferSize), windowSize(windowSize), fileName(filename){}

        inline uint32_t getBufferSize(){return this->bufferSize;}
        inline uint32_t getWindowSize(){return this->windowSize;}
        inline char *getFileName(){return this->fileName;}
};

/*==================sender/builder===========================================*/
class RCopyPacketBuilder{
    private:
        RCopyPacketBuilder(){}
    public:
        static RCopyPacket Build(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen){
            return RCopyPacket(htonl(seqNum), flag, payload, payloadLen);
        }
        static RCopyPacket BuildSetupPacket(uint32_t buffSize, uint32_t windowSize, const char *fileName){

           uint8_t payload[MAX_PAYLOAD_LEN];
           memset(payload, 0, MAX_PAYLOAD_LEN);
           memcpy(payload, &buffSize, sizeof(buffSize));
           memcpy(payload+sizeof(buffSize), &windowSize, sizeof(windowSize));
           memcpy(payload+sizeof(buffSize)+sizeof(windowSize), fileName, 100); // todo make var for max filename 100
           return RCopyPacket(htonl(seqNum), FILENAME_PACKET, payload, MAX_SETUP_PAYLOAD_LEN);
            
        }
};

class RCopyPacketSender{
    private:
        RCopyPacketSender(){}
    public:
        static ssize_t Send(int socketNum, RCopyPacket &packet, Connection &con){
            return safeSendToErr(socketNum, 
                                 packet.getRawPacket(), 
                                 packet.getPacketLen(), 
                                 0, 
                                 (struct sockaddr*)con.getRemote(),
                                 *con.getRemoteLen()
                                 );
       }

};

/*===========================parser/recv========================================*/

class RCopyPacketParser{
    private:
        RCopyPacketParser(){}
    public:
        static RCopyPacket Parse(uint8_t *packet, int payloadLen) throw(CorruptPacketException){
            uint32_t seqNum;
            uint8_t flag;
            uint8_t *payload;
            // step 1 - extract fields and tests checksum
            if(in_cksum((unsigned short *)packet, payloadLen+HDR_LEN)!=0)
                throw CorruptPacketException("CRC error");
            // step 2 - fill the RcopyPacket (assume correct)
            memcpy(&seqNum, payload, sizeof(seqNum));
            memcpy(&flag, packet+sizeof(seqNum)+sizeof(uint16_t), sizeof(flag));
            payload = packet + HDR_LEN;
            return RCopyPacket(ntohl(seqNum), flag, payload, payloadLen);
        }
        static RCopySetupPacket ParseSetup(uint8_t *packet) throw(CorruptPacketException){
            uint32_t seqNum;
            uint8_t flag;
            uint8_t *payload;
            // setup vars
            char fileName[MAX_FILENAME_LEN];
            uint32_t bufferSize, windowSize;
            memset(fileName, 0, MAX_FILENAME_LEN);
            // step 1 - extract fields and tests checksum
            if(in_cksum((unsigned short *)packet, MAX_SETUP_PAYLOAD_LEN+HDR_LEN)!=0)
                throw CorruptPacketException("CRC error");
            // step 2 - fill the RcopyPacket (assume correct)
            // TODO : check if setup packet
            memcpy(&seqNum, payload, sizeof(seqNum));
            memcpy(&flag, packet+sizeof(seqNum)+sizeof(uint16_t), sizeof(flag));
            payload = packet + HDR_LEN;
            memcpy(&bufferSize, payload, sizeof(bufferSize));
            memcpy(&windowSize, payload+sizeof(bufferSize), sizeof(windowSize));
            memcpy(&fileName, payload+sizeof(bufferSize)+sizeof(windowSize), MAX_FILENAME_LEN);
            //return RCopySetupPacket(ntohl(seqNum), flag, payload, payloadLen);
            return RCopySetupPacket(bufferSize, windowSize, fileName, payload);
        }

};

class RCopyPacketReciever{
    private:
        RCopyPacketReciever(){}
    public:
        static RCopyPacket Recieve(int payloadLen, Connection &con) throw(CorruptPacketException){
            static uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
            memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
            // Step 1 - recv packet
            safeRecvfrom(con.getSocketNumber(), temp, payloadLen+HDR_LEN, 0,(struct sockaddr*)con.getRemote(), con.getRemoteLen());
            // Step 2 - parse packet
            try{
               return RCopyPacketParser::Parse(temp, payloadLen);
            }catch(CorruptPacketException &e){
                throw e;
            }
        }
        static RCopySetupPacket RecieveSetup(Connection &con) throw(CorruptPacketException){
            static uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
            memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
            // Step 1 - recv packet
            safeRecvfrom(con.getSocketNumber(), temp, MAX_SETUP_PAYLOAD_LEN+HDR_LEN, 0,(struct sockaddr*)con.getRemote(), con.getRemoteLen());
            // Step 2 - parse packet
            try{
               return RCopyPacketParser::ParseSetup(temp);
            }catch(CorruptPacketException &e){
                throw e;
            }
        }


};


#endif
