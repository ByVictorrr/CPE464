#ifndef PACKET_H_
#define PACKET_H_
#include <iostream>
#include <utility>

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
    FILNAME_RESPONSE_PACKET,
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

        inline void clearPacket(){
            memset(payload, 0, MAX_PAYLOAD_LEN);
            header.clear();
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


};

class RCopyPacketBuilder{
    private:
        // Helper functions
        RCopyPacketBuilder(){}
    public:
        // <size, packet>
        static RCopyPacket buildPacket(uint8_t *packet, int payloadLen){
            uint32_t seqNum;
            uint8_t flag;
            uint8_t *payload;
            memcpy(&seqNum, packet, sizeof(seqNum));
            // TODO : DO i have to convert host<->netowrk for flag
            memcpy(&flag, packet+sizeof(seqNum)+sizeof(uint16_t), sizeof(flag));
            payload = packet + HDR_LEN;
            return RCopyPacket(htonl(seqNum), flag, payload, payloadLen);
        }
};

class RCopyPacketParser{
    private:
        RCopyPacketParser(){}
    public:
        static RCopyPacket parsePacket(uint8_t *packet, int payloadLen){
            uint32_t seqNum;
            uint8_t flag;
            uint8_t *payload;
            // step 1 - extract fields and tests checksum
            if(in_cksum((unsigned short *)packet, payloadLen+HDR_LEN)!=0)
                throw "Packet bits flipped";
            // step 2 - fill the RcopyPacket (assume correct)
            memcpy(&seqNum, payload, sizeof(seqNum));
            memcpy(&flag, packet+sizeof(seqNum)+sizeof(uint16_t), sizeof(flag));
            payload = packet + HDR_LEN;
            return RCopyPacket(ntohl(seqNum), flag, payload, payloadLen);
        }

};
#endif
