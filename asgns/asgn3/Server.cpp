#include <thread>
#include <iostream>
#include <string>
#include "networks.hpp"
#include "Packet.hpp"
#include "safe_sys_calls.h"
#include "Utils.hpp"
#include "Server.hpp"
#include <fcntl.h>
 
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>

class RCopyPacketReciever;
class ServerConnection;
class ServerArgs;
/**************Server**********************/

Server::Server(ServerArgs &args)
: gateway(args.getPortNumber()), args(args)
{
    sendtoErr_init(args.getErrorPercent(), DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
}

void Server::serve(){
    pid_t pid;
    start:
        // Step 1 - see if a new client is trying to connect
        if(safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0) == true){
            // create thread 
            try{
                RCopySetupPacket setup = RCopyPacketReciever::RecieveSetup(this->gateway);

                /*
                RCopyPacket p = RCopyPacketBuilder::Build(0, FILENAME_PACKET_OK, NULL, MAX_SETUP_PAYLOAD_LEN+HDR_LEN);
                RCopyPacketSender::Send(p, gateway);
                */
                if((pid=safe_fork()) == 0){
                    /*
                    RCopyPacketDebugger::println(setup);                       
                    RCopyPacket p = RCopyPacketBuilder::Build(0, FILENAME_PACKET_OK, NULL, MAX_SETUP_PAYLOAD_LEN+HDR_LEN);
                    RCopyPacketSender::Send(p, gateway);
                    */

                    ServerThread t(setup, this->gateway);
                    t.join();
                }
            }catch(CorruptPacketException &e){
                goto start;
            }
        }
        /*
        while(waitpid(-1, NULL, WNOHANG) > 0){
            std::cout << "processed wait" << std::endl;
        }
        */
        
    goto start;
}





/*******************ServerThread****************/

ServerThread::ServerThread(RCopySetupPacket setup, ServerConnection &c) 
: gateway(c)
{
    this->processRCopy(setup);
}
       
/***util funcs****/
 void ServerThread::readFile(uint8_t *payload, size_t *lenRead) throw (ReadEOFException){
     std::cout << "In readfile";
    *lenRead = 0;
    while(*lenRead != this->bufferSize){
        if(safe_read(this->file, payload+*lenRead, 1) == 0){
            std::cout << "READ: " << *lenRead << "bytes" << std::endl;
            throw ReadEOFException("EOF Packet"); 

        }
        *lenRead+=1;
    }
    std::cout << "READ: " << *lenRead << "bytes" << std::endl;
}


RCopyPacket ServerThread::buildDataPacket(uint32_t seqNum) throw (ReadEOFException){
    uint8_t payload[MAX_PAYLOAD_LEN];
    memset(payload, 0, MAX_PAYLOAD_LEN);
    size_t lenRead;
    try{
        this->readFile(payload, &lenRead);

        return RCopyPacketBuilder::Build(seqNum, DATA_PACKET, payload, lenRead);
    }catch(ReadEOFException &e){
        // EOF_PACKET may contain data
        return RCopyPacketBuilder::Build(seqNum, EOF_PACKET, payload, lenRead);
    }
}

ssize_t ServerThread::sendPacket(RCopyPacket &packet){
    return RCopyPacketSender::Send(packet, this->gateway);
}



/**************state func****************/
state_t ServerThread::sendData(){
    state_t ret;
    static bool recvdEOF=false;
    // Case 1 - window is closed (waiting on message/cant send anymore data)
    if(this->window->isClosed() || recvdEOF){
        std::cout << "window is closed" << std::endl;
        return WAITING;
    }else{                    
        // Case 2 - window is open (still can send data)
        uint32_t currSeqNum = this->window->getCurrent();
        RCopyPacket &&built = this->buildDataPacket(currSeqNum);
        uint8_t flag = built.getHeader().getFlag();
        this->window->insert(built); // put into the window

        switch (flag)
        {
            // Case 2.1 - could be the last packet read
            case EOF_PACKET:
            {
                std::cout << "IN EOF_PACKET" << std::endl;
                this->sendPacket(built);
                recvdEOF=true;
                return WAITING; //
            }
            break;
            // Case 2.2 - packet read was a data packet (not eof)
            case DATA_PACKET:
            {
                this->sendPacket(built);
                this->window->setCurrent(currSeqNum+1);
                // Case 2.2.1 - see if we recvd any data(rr/srej)
                if(safeSelectTimeout(this->gateway.getSocketNumber(), 0, 0)) 
                    return RECV_DATA;
                else
                    return SEND_DATA;
            }
            break;        
            default:
                break;
        }
    }
}
    
/* Only called if select is called and its true */
state_t ServerThread::receiveData(){

    RCopyPacket recvd;
    uint8_t flag;
    uint32_t seqNum;
    try{
        recvd = RCopyPacketReciever::Recieve(this->bufferSize, this->gateway);
    }catch(CorruptPacketException &e){
        return WAITING; 
    }

    flag=recvd.getHeader().getFlag();
    seqNum=recvd.getHeader().getSequenceNumber();
    // Case 1 - if the flag if srej, send pkt again and goto sendData
    switch (flag)
    {
    case SREJ_PACKET:
    {
        RCopyPacket &p = this->window->getPacket(recvd.getHeader().getSequenceNumber());
        if(this->window->inWindow(p.getHeader().getSequenceNumber()))
            this->sendPacket(p); // p is not acked yet
        return SEND_DATA;
        
    }
    break; 
    case RR_PACKET:
    {
        /* Note: if all packets are in the window and the window is closed
            from: 
                The sliding of the window clears up the spot and {isAcked = false, inWindow=false}

                Therefore the Window might open after this below segment
                    If the seqNum is lower part of the window
        */

        if(this->window->inWindow(seqNum)){
        this->window->setIsAcked(seqNum); // packet with seqNum is acked
        // Case 1 - where left most packet is the revd one
        if(this->window->getLower() == seqNum){
            // Case 2 - check to see if any adjacent are acked
            int upper=this->window->getUpper();
            int lower=this->window->getLower();
            for(int i=lower; i<=upper; i++){                    
                // Case 2.2 - if the adjacent isnt acked stop slidding
                if(!this->window->isAcked(i)){
                    break;
                }
                this->window->slide(i+1);
            }
        // Case 2 - where the packet recvd isnt the leftmost
        }else{
            this->window->setIsAcked(seqNum);
        }
        }
        return SEND_DATA;
    }
    break;
    case EOF_PACKET_ACK:
    {
        return DONE;
    }
    break;
    default:
        ;
        break;
    }
    return SEND_DATA;
}


state_t ServerThread::waiting(){

    int count = 0;
    start:
    std::cout << "waiting" << std::endl;
    // Case 1 - timeout occured
    if(!safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0)){
        // send the lowest unacked packet
        
        std::cout << "==========================" << std::endl;
        std::cout << "SENDING LOWEST unacked packet" << std::endl;
        RCopyPacket &p = this->window->getPacket(this->window->getLower());
        std::cout << this->window->getLower() << std::endl;
        std::cout << "=================" << std::endl;
        std::cout << "" << std::endl;
        this->sendPacket(p);
        if(count++ > 9){
            std::cout << "bye" << std::endl;
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


    std::cout << setup << std::endl; // debug
    // step 1 - parse setup packet
    this->bufferSize = setup.getBufferSize();
    this->window = new Window(setup.getWindowSize());
    
    // Step 2 - go through the process
    std::cout << "new server thread" << std::endl;

    while(1){
        switch (state)
        {
            case FILENAME:
            {
                if((file = open(fileName.c_str(), O_RDONLY)) < NULL){
                    RCopyPacket p = RCopyPacketBuilder::Build(-1, FILENAME_PACKET_BAD, NULL, bufferSize);
                    // send bad filename 
                    this->sendPacket(p);
                    state = DONE;
                }else{
                    RCopyPacket p = RCopyPacketBuilder::Build(-1, FILENAME_PACKET_OK, NULL, bufferSize);
                    /*
                    std::cout << "FLAG: " << std::to_string(p.getRawPacket()[6]) << std::endl;
                    */
                    //RCopyPacketDebugger::println(p);
                    this->sendPacket(p);
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
                state = receiveData();
            }
            break;
            case DONE:
            {
                safe_close(this->file);
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
    exit(EXIT_SUCCESS);
}


