#ifndef PACKET_H_
#define PACKET_H_
#include <iostream>
#include <utility>
#include <string.h>
#include <arpa/inet.h>
#include "Exception.hpp"
#include "cpe464.h"
#include "networks.hpp"

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
        /* Constructors */
        RCopyHeader(uint32_t seqNum, uint16_t checksum, uint8_t flag);
        /* Settters */
        void setSequenceNumber(uint32_t seqNum);
        void setChecksum(uint16_t checksum);
        void setFlag(uint8_t flag);
        /* Getters */
        uint32_t getSequenceNumber();
        uint16_t getChecksum();
        uint8_t getFlag();

        /* Utility functions */
        void clear();
        int size();
};


class RCopyPacket{
    protected:
        RCopyHeader header;
        uint8_t payload[MAX_PAYLOAD_LEN];
        int payloadSize;
        uint16_t computeChecksum(RCopyHeader &header, uint8_t *payload, int payloadSize);
    public:
        /* Constructors */
        RCopyPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadSize);
        RCopyPacket();

        /* Getters */
        RCopyHeader &getHeader();
        uint8_t *getPayload();
        int getPayloadSize();

        /* Utils */
        uint8_t *getRawPacket();
        int size();
        void clear();
};


class RCopySetupPacket: public RCopyPacket{
    protected:
        uint32_t bufferSize;
        uint32_t windowSize;
        const char *fileName;
    public:
        static const int MAX_PAYLOAD_SIZE = 108;
        /* Constructor */
        RCopySetupPacket(uint32_t bufferSize, uint32_t windowSize, const char *filename, uint8_t* payload);
        /* Getters */
        uint32_t getBufferSize();
        uint32_t getWindowSize();
        const char *getFileName();
};

/*==================sender/builder===========================================*/
class RCopyPacketBuilder{
    private:
        RCopyPacketBuilder();
    public:
        static RCopyPacket Build(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen);
        static RCopyPacket BuildSetupPacket(uint32_t buffSize, uint32_t windowSize, const char *fileName);
};

class RCopyPacketSender{
    private:
        RCopyPacketSender();
    public:
        static ssize_t Send(RCopyPacket &packet, Connection &con);
};

/*===========================parser/recv========================================*/

class RCopyPacketParser{
    private:
        RCopyPacketParser();
    public:
        static RCopyPacket Parse(uint8_t *packet, int payloadLen)throw(CorruptPacketException);
        static RCopySetupPacket ParseSetup(uint8_t *packet) throw(CorruptPacketException);

};

class RCopyPacketReciever{
    private:
        RCopyPacketReciever(){}
    public:
        static RCopyPacket Recieve(int payloadLen, Connection &con) throw(CorruptPacketException);
        static RCopySetupPacket RecieveSetup(Connection &con) throw(CorruptPacketException);

};


#endif
