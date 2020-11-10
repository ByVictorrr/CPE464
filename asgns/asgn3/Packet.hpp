#ifndef PACKET_H_
#define PACKET_H_
#include <iostream>
#include <utility>
#include "Exception.hpp"

#define HDR_LEN 7
#define MAX_PAYLOAD_LEN 1400

enum FLAGS{
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
    EOF_PACKET
};

class RCopyHeader{
    private:
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
        /* Utility functions */
        inline void clear(){ 
            this->sequenceNum = 0;
            this->checksum = 0;
            this->flag = 0;
        }
};


class RCopyPacket{
    private:
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
        inline int getPacketLen(){
            return this->payloadSize+HDR_LEN;
        }


};

/*==================sender/builder===========================================*/
class RCopyPacketBuilder{
    private:
        RCopyPacketBuilder(){}
    public:
        static RCopyPacket Build(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen){
            return RCopyPacket(htonl(seqNum), flag, payload, payloadLen);
        }
        static RCopyPacket BuildSetupPacket(uint32_t seqNum, uint8_t flag, 
                                            uint32_t buffSize, uint32_t windowSize, 
                                            const char *fileName){

           uint8_t payload[MAX_PAYLOAD_LEN];
           memset(payload, 0, MAX_PAYLOAD_LEN);
           memcpy(payload, &buffSize, sizeof(buffSize));
           memcpy(payload+sizeof(buffSize), &windowSize, sizeof(windowSize));
           memcpy(payload+sizeof(buffSize)+sizeof(windowSize), fileName, strlen(fileName));
           return RCopyPacket(htonl(seqNum), flag, payload, 2*sizeof(uint32_t)+strlen(fileName));
            
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

};

class RCopyPacketReciever{
    private:
        RCopyPacketReciever(){}
    public:
        static RCopyPacket Recieve(int skt, int payloadLen, Connection &con) throw(CorruptPacketException){
            static uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
            memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
            // Step 1 - recv packet
            safeRecvfrom(skt, temp, payloadLen+HDR_LEN, 0,(struct sockaddr*)con.getRemote(), con.getRemoteLen());
            // Step 2 - parse packet
            try{
               return RCopyPacketParser::Parse(temp, payloadLen);
            }catch(CorruptPacketException &e){
                throw e;
            }
        }

};


#endif
