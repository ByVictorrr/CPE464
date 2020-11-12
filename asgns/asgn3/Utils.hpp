#ifndef UTILS_H_
#define UTILS_H_

#include "Packet.hpp"

class RCopyPacket;
class Window{
    private:
        int upper, lower, current;
        int size;
        RCopyPacket *packets;
        bool *valid;
        int getIndex(uint32_t seqNum);
    public:
        Window(int size);
        ~Window();
        /************Getters/Setters**************/
        int getLower();
        int getUpper();
        int getCurrent();
        void setLower(int seqNum);
        void setUpper(int seqNum);
        void setCurrent(int seqNum);

        /*************Utility functions***********/
        bool inWindow(uint32_t seqNum);

        /* Function to create Circular queue */
        void insert(RCopyPacket &value);
        void remove(uint32_t seqNum);

        RCopyPacket &getPacket(uint32_t seqNum);
        bool isClosed();
        void slide(uint32_t toSeqNum);

        
};


#endif