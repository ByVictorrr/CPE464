#ifndef UTILS_H_
#define UTILS_H_

#include "Packet.hpp"

class RCopyPacket;
class Window{
    private:
        uint32_t upper, lower, current;
        int size;
        RCopyPacket *packets;
        bool *valid;
        int getIndex(uint32_t seqNum);
    public:
        Window(int size);
        ~Window();
        /************Getters/Setters**************/
        uint32_t getLower();
        uint32_t getUpper();
        uint32_t getCurrent();
        void setLower(uint32_t seqNum);
        void setUpper(uint32_t seqNum);
        void setCurrent(uint32_t seqNum);

        /*************Utility functions***********/
        bool inWindow(uint32_t seqNum);

        /* Function to create Circular queue */
        void insert(RCopyPacket &value);
        void remove(uint32_t seqNum);

        RCopyPacket &getPacket(uint32_t seqNum);
        bool isClosed();
        void slide(uint32_t toSeqNum);
        int getSize();

        
};


#endif