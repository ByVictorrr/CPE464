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
        static const int HEADER_SIZE = 7;
        /* Constructors */
        RCopyHeader(uint32_t seqNum, uint16_t checksum, uint8_t flag);
        RCopyHeader(){}
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
        friend std::ostream &operator<<(std::ostream &os, RCopyHeader &h) {
            os << "{seqNum:" << h.sequenceNum 
               << ",checksum:" << h.checksum
               << ",flag:" << std::to_string(h.flag)
               << "}";
            return os;

        }
       
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
        RCopyPacket(){}

        /* Getters */
        RCopyHeader &getHeader();
        uint8_t *getPayload();
        int getPayloadSize();

        /* Utils */
        uint8_t *getRawPacket();
        int size();
        void clear();
        friend std::ostream &operator<<(std::ostream &os, RCopyPacket &p) {
            os << "packet={header:" << p.header << "}";
            return os;
        }
};


class RCopySetupPacket: public RCopyPacket{
    protected:
        uint32_t bufferSize;
        uint32_t windowSize;
        std::string fileName;
    public:
        static const int MAX_PAYLOAD_SIZE = 108;
        /* Constructor */
        RCopySetupPacket(uint32_t bufferSize, uint32_t windowSize, const char *filename, uint8_t* payload);
        RCopySetupPacket(){}
        /* Getters */
        uint32_t getBufferSize();
        uint32_t getWindowSize();
        std::string &getFileName();
        friend std::ostream &operator<<(std::ostream &os, RCopySetupPacket &p) {
            os << "packet={header:" << p.header 
               << ",bufferSize:" << p.bufferSize
               << ",windowSize:" << p.windowSize
               << ",fromFileName" << p.fileName << "}";
            return os;
        }

};

class RCopyACKPacket: public RCopyPacket{
    public:
        RCopyACKPacket(uint32_t seqNum, uint8_t flag, uint8_t *payload);
};

/*==================sender/builder===========================================*/
class RCopyPacketBuilder{
    private:
        //RCopyPacketBuilder();
    public:
        static RCopyPacket Build(uint32_t seqNum, uint8_t flag, uint8_t *payload, int payloadLen);
        static RCopySetupPacket BuildSetup(uint32_t bufferSize, uint32_t windowSize, const char *fileName);
        static RCopyACKPacket BuildACK(uint32_t seqNum, uint8_t flag);
};

class RCopyPacketSender{
    private:
        //RCopyPacketSender();
    public:
        static ssize_t Send(RCopyPacket &packet, Connection &con);
        static ssize_t SendSetup(RCopySetupPacket &packet, Connection &con);
        static ssize_t SendACK(RCopyACKPacket &packet, Connection &con);
};

/*===========================parser/recv========================================*/

class RCopyPacketParser{
    private:
        //RCopyPacketParser();
    public:
        static RCopyPacket Parse(uint8_t *packet, int payloadLen)throw(CorruptPacketException);
        static RCopySetupPacket ParseSetup(uint8_t *packet) throw(CorruptPacketException);
        static RCopyACKPacket ParseACK(uint8_t *packet) throw(CorruptPacketException);
};

class RCopyPacketReciever{
    private:
        //RCopyPacketReciever(){}
    public:
        static RCopyPacket Recieve(int payloadLen, Connection &con) throw(CorruptPacketException);
        static RCopySetupPacket RecieveSetup(Connection &con) throw(CorruptPacketException);
        static RCopyACKPacket RecieveACK(Connection &con) throw(CorruptPacketException);// fixed payloadlen
};

class RCopyPacketDebugger{
    private:
    public:
        inline static void println(RCopyPacket &p){
            std::cout << p << std::endl;
        }
        inline static void println(RCopySetupPacket &p){
            std::cout << p << std::endl;
        }



};
#endif
