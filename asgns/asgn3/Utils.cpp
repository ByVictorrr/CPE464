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

    this->packets = new RCopyPacket[size]; // +1 because of size = 0
    this->_inWindow = new bool[size];
    this->_isAcked = new bool[size];
}
Window::~Window(){
    delete [] packets;
    delete [] _inWindow;
    delete [] _isAcked;
}
/************Getters/Setters**************/
uint32_t Window::getLower(){return this->lower;}
uint32_t Window::getUpper(){return this->upper;}
uint32_t Window::getCurrent(){return this->current;}
void Window::setLower(uint32_t seqNum){this->lower=seqNum;}
void Window::setUpper(uint32_t seqNum){this->upper=seqNum;}
void Window::setCurrent(uint32_t seqNum){this->current=seqNum;}
void Window::setIsAcked(uint32_t seqNum){this->_isAcked[this->getIndex(seqNum)] = true;}
/*************Utility functions***********/
bool Window::inWindow(uint32_t seqNum){
    return this->_inWindow[getIndex(seqNum)];
}
bool Window::isAcked(uint32_t seqNum){
    return this->_isAcked[getIndex(seqNum)];
}

/* Function to create Circular queue */
void Window::insert(RCopyPacket &value){ 
    uint32_t seqNum = value.getHeader().getSequenceNumber();
    int index = getIndex(seqNum);
    this->current = seqNum;
    this->packets[index] = value;
    this->_inWindow[index] = true;
    this->_isAcked[index] = false;
} 
/* Helper of slide */
void Window::remove(uint32_t seqNum){
    int index = getIndex(seqNum);
    this->_inWindow[index] = false;
    this->_isAcked[index] = false;
}

RCopyPacket &Window::getPacket(uint32_t seqNum){
    int index = getIndex(seqNum);
    return this->packets[index];
}

bool Window::isClosed(){
    for(int i=this->lower; i < this->upper+1; i++)
        if(!this->inWindow(i))
            return false;
    return true;
}

void Window::slide(uint32_t toSeqNum){
    for(int i= this->lower; i < toSeqNum; i++){
        this->remove(i);
    }
    this->lower = toSeqNum;
    this->upper = this->lower+(this->size-1);
    if(this->current < toSeqNum){
        this->current = toSeqNum;
    }
}

int Window::getSize(){
    return this->size;
}        
