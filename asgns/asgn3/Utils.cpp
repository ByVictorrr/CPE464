#include "Packet.hpp"
#include "Utils.hpp"

int Window::getIndex(uint32_t seqNum){
    return (seqNum - 1) % this->size;
}
Window::Window(int size){
    this->lower = 0;
    this->current = 0;
    this->upper = size - 1;
    this->size = size;
    this->packets = new RCopyPacket[size];
    this->valid = new bool[size];
}
Window::~Window(){
    delete [] packets;
    delete [] valid;
}
/************Getters/Setters**************/
uint32_t Window::getLower(){return this->lower;}
uint32_t Window::getUpper(){return this->upper;}
uint32_t Window::getCurrent(){return this->current;}
void Window::setLower(uint32_t seqNum){this->lower=seqNum;}
void Window::setUpper(uint32_t seqNum){this->upper=seqNum;}
void Window::setCurrent(uint32_t seqNum){this->current=seqNum;}

/*************Utility functions***********/
bool Window::inWindow(uint32_t seqNum){
    return this->valid[getIndex(seqNum)];
}


/* Function to create Circular queue */
void Window::insert(RCopyPacket &value){ 
    int index;
    uint32_t seqNum;
    seqNum = value.getHeader().getSequenceNumber();
    index = getIndex(seqNum);
    if(!inWindow(seqNum)){
        this->packets[index] = value;
        this->valid[index] = true;
    }else{
        std::cout << "Index in window " + std::to_string(index) + " filled already" << std::endl;
    }
} 
void Window::remove(uint32_t seqNum){
    int index = getIndex(seqNum);
    this->valid[index] = false;
}

RCopyPacket &Window::getPacket(uint32_t seqNum){
    int index = getIndex(seqNum);
    if(inWindow(seqNum)){
        return this->packets[index];
    }
}
bool Window::isClosed(){
    for(int i=this->lower; i < this->upper+1; i++)
        if(!inWindow(i))
            return false;
    return true;
}
void Window::slide(uint32_t toSeqNum){
    for(int i= toSeqNum; i < toSeqNum; i++){
        remove(i);
    }
    this->lower = toSeqNum;
    this->upper = this->lower+this->size-1;
    if(this->current < toSeqNum){
        this->current = toSeqNum;
    }
}

int Window::getSize(){
    return this->size;
}        
