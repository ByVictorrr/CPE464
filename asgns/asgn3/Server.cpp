#include <thread>
#include <iostream>
#include <string>
#include "networks.hpp"
#include "Packet.hpp"
#include "safe_sys_calls.h"
#include "Utils.hpp"
#include "Server.hpp"

class RCopyPacketReciever;
class ServerConnection;
class ServerArgs;
/**************Server**********************/

Server::Server(ServerArgs &args)
: gateway(args.getPortNumber()), args(args)
{}

void Server::serve(){
    start:
        // Step 1 - see if a new client is trying to connect
        if(safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0) == true){
            // create thread 
            try{
                RCopySetupPacket setup = RCopyPacketReciever::RecieveSetup(this->gateway);
                std::cout << setup << std::endl;
                ServerThread t(setup, args);
                t.join();
            }catch(CorruptPacketException &e){
                ; // kill thread
            }
        }
    goto start;
}





/*******************ServerThread****************/

ServerThread::ServerThread(RCopySetupPacket setup, ServerArgs &args) 
: errorPercent(args.getErrorPercent()), 
  gateway(args.getPortNumber())
{

    thread = new std::thread([this, setup]{this->processRCopy(setup);});

 }
       
/***util funcs****/
void ServerThread::readFile(uint8_t *payload) throw (ReadEOFException){
    if(fread(payload, sizeof(uint8_t), this->bufferSize, file) != this->bufferSize){
        throw ReadEOFException("EOF Packet"); 
    }
}


RCopyPacket ServerThread::buildDataPacket(uint32_t seqNum) throw (ReadEOFException){
    uint8_t payload[MAX_PAYLOAD_LEN];
    memset(payload, 0, MAX_PAYLOAD_LEN);
    try{
        readFile(payload);
        return RCopyPacketBuilder::Build(seqNum, DATA_PACKET, payload, this->bufferSize);
    }catch(ReadEOFException &e){
        // EOF_PACKET may contain data
        return RCopyPacketBuilder::Build(seqNum, EOF_PACKET, payload, this->bufferSize);
    }
}

ssize_t ServerThread::sendPacket(RCopyPacket &packet){
    return RCopyPacketSender::Send(packet, this->gateway);
}


/**************state func****************/
state_t ServerThread::sendData(){

    // Case 1 - window is closed (waiting on message/cant send anymore data)
    if(this->window->isClosed()){
        return WAITING;
    }else{                    
    // Case 2 - window is open (still can send data)
        flag_t flag;
        uint32_t currSeqNum = this->window->getCurrent();
        RCopyPacket &&built = this->buildDataPacket(currSeqNum);
        // Case 2.1 - check to see if the built packet is EOF packet
        if((flag=(flag_t)built.getHeader().getFlag())==EOF_PACKET){
            return WAITING;
        // Case 2.2 - check to see if its a regulare data packet
        }else if(flag == DATA_PACKET){
            this->window->setCurrent(currSeqNum+1);
            this->window->insert(built);
            if(safeSelectTimeout(this->gateway.getSocketNumber(), 0, 0)) 
                return RECV_DATA;
            else
                return SEND_DATA;
        }
    }
}
    

state_t ServerThread::receiveData(){

    RCopyPacket recvd;
    flag_t flag;
    try{
        recvd = RCopyPacketReciever::Recieve(this->bufferSize, this->gateway);
    }catch(CorruptPacketException &e){
        return WAITING; 
    }

    // Case 1 - if the flag if srej, send pkt again and goto sendData
    if((flag=(flag_t)recvd.getHeader().getFlag()) == SREJ_PACKET){
        RCopyPacket &p = this->window->getPacket(recvd.getHeader().getSequenceNumber());
        this->sendPacket(p);
    }else if(flag == RR_PACKET){
        this->window->slide(recvd.getHeader().getSequenceNumber()+1);
    }else if(flag == EOF_PACKET_ACK){
        return DONE;
    }
    return SEND_DATA;
}


state_t ServerThread::waiting(){

    int count = 0;
    start:
    // Case 1 - timeout occured
    if(!safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0)){
        // send the lowest unacked packet
        RCopyPacket &p = this->window->getPacket(this->window->getLower());
        this->sendPacket(p);
        if(count++ > 9){
            return DONE;
        }
        goto start;
    // Case 2 - found data;
    }else{
        return RECV_DATA;
    }
}



void ServerThread::processRCopy(RCopySetupPacket setup)
{
    std::string &fileName = setup.getFileName();
    state_t state = FILENAME;


    // step 1 - parse setup packet
    this->bufferSize = setup.getBufferSize();
    this->window = new Window(setup.getWindowSize());
    // Step 2 - go through the process
    while(1){
        switch (state)
        {
            case FILENAME:
            {
                if((file = safe_fopen(fileName.c_str(), "r+")) == NULL){
                    RCopyPacket p = RCopyPacketBuilder::Build(0, FILENAME_PACKET_BAD, NULL, bufferSize);
                    // send bad filename 
                    sendPacket(p);
                    state = DONE;
                }else{
                    RCopyPacket p = RCopyPacketBuilder::Build(0, FILENAME_PACKET_OK, NULL, bufferSize);
                    sendPacket(p);
                    state=SEND_DATA;
                    // fill the window
                }
              
            }
            break;
            case SEND_DATA:
            {
                // send first data packet
                state = sendData();
            }
            break;
            case WAITING:
            {
                state = waiting();
            }
            break;
            case RECV_DATA:
            {

            }
            break;
            case DONE:
            {
                safe_fclose(this->file);
                safe_close(this->gateway.getSocketNumber());
                return;
            }
            break;
            default:
                break;
            }
    }
    
}

void ServerThread::join()
{
    thread->join();
}


