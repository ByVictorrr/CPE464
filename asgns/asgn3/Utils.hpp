#ifndef UTILS_H_
#define UTILS_H_

#include "Packet.hpp"

class Window{
    private:
        int upper, lower, current;
        int size;
        RCopyPacket *packets;
        bool *valid;
        inline int getIndex(uint32_t seqNum){
            return (seqNum - 1) % this->size;
        }
    public:
        Window(int size){
            this->lower = -1;
            this->current = -1;
            this->upper = size - 1;
            this->size = size;
            this->packets = new RCopyPacket[size];
            this->valid = new bool[size];
        }
        bool isValid(uint32_t seqNum){
            return this->valid[getIndex(seqNum)];
        }
        /* Function to create Circular queue */
        void insert(RCopyPacket &value){ 
            int index;
            uint32_t seqNum;
            seqNum = value.getHeader().getSequenceNumber();
            index = getIndex(seqNum);
            if(isValid(seqNum)){
                this->packets[index] = value;
                this->valid[index] = true;
            }else{
                std::cout << "Index in window " + std::to_string(index) + " filled already" << std::endl;
            }
        } 
        void remove(uint32_t seqNum){
            int index = getIndex(seqNum);
            this->valid[index] = false;
        }
        RCopyPacket &getPacket(uint32_t seqNum){
            int index = getIndex(seqNum);
            if(isValid(seqNum)){
                return this->packets[index];
            }
        }
        void slide(uint32_t toSeqNum){
            for(int i= toSeqNum; i < toSeqNum; i++){
                remove(i);
            }
            this->lower = toSeqNum;
            this->upper = this->lower+this->size-1;
            if(this->current < toSeqNum){
                this->current = toSeqNum;
            }
        }


        
};


#endif