#include "RCopy.hpp"
/********* Constructor/destructor***********/
RCopy::RCopy(RCopyArgs & cmdArgs)
: args(cmdArgs), window(cmdArgs.getWindowSize()), 
  gateway(args.getRemoteMachine(), args.getPort()), toFile(NULL){}



ssize_t RCopy::sendACKPacket(RCopyACKPacket &packet){
    return RCopyPacketSender::SendACK(packet, this->gateway);
}

ssize_t RCopy::sendSetupPacket(RCopySetupPacket &p){
    return RCopyPacketSender::SendSetup(p, this->gateway);
}

/**
 * 
 */
RCopyPacket RCopy::recievePacket() throw (CorruptPacketException){
    try{

        RCopyPacket rr = RCopyPacketReciever::Recieve(this->args.getBufferSize(), this->gateway);
        if(rr.getHeader().getFlag() == EOF_PACKET){
            std::cout << "RECV_PACKET : EOF payload length : " << rr.getPayloadSize() << std::endl;
        }
        return rr;
    }catch(CorruptPacketException &e){
        throw e;
    }
}

RCopyACKPacket RCopy::buildACKPacket(uint32_t seqNum, uint8_t flag){
    return RCopyPacketBuilder::BuildACK(seqNum, flag);
}


size_t RCopy::writePacketToFile(RCopyPacket &p){
    size_t len;
    std::cout << "PAYLAOD SIZE IN write function "<< p.getPayloadSize() << std::endl;
    if((len=fwrite((void*)p.getPayload(), (size_t)sizeof(uint8_t), p.getPayloadSize(), 
                   this->toFile)) != (size_t)p.getPayloadSize()){
                        std::cerr << "problem writing " << std::endl;
                        return 0;
                    }
        std::cout << "wrote " << len << std::endl; 
        return len;
}



//============= State functions ===============//
state_t RCopy::sendFileName(){
    uint8_t flag;
    state_t ret = FILENAME;
    RCopyPacket recievedPacket;
    RCopySetupPacket builtPacket = RCopyPacketBuilder::BuildSetup( 
                                                            (uint32_t)args.getBufferSize(), 
                                                            (uint32_t)args.getWindowSize(), 
                                                            args.getFromFileName()
                                                            );
    this->sendSetupPacket(builtPacket);
    if(safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0)){
        // read the packet and check crc
        try{
            recievedPacket = recievePacket();
            
            if((flag=recievedPacket.getHeader().getFlag()) == FILENAME_PACKET_BAD){
                std::cerr << "File " + std::string(args.getFromFileName()) + "Not found" << std::endl;
                ret = DONE;
            }else if(flag == FILENAME_PACKET_OK){
                ret = FILENAME_OK;
            }
        }catch(CorruptPacketException &e){
            RCopyPacketDebugger::println(recievedPacket);
            ret = FILENAME;
        }
    }

    return ret;
}

state_t RCopy::receieveData(){
    uint32_t seqNum;
       /* Case 1 - no data packet waiting */
    if(!safeSelectTimeout(this->gateway.getSocketNumber(), 10, 0))
        return DONE;
    // Case 2 - data packet waiting
    try{
        std::cout << "===================================\n";
        std::cout << this->window.getLower() << std::endl;
        RCopyPacket &&recvPacket = recievePacket();
        seqNum = recvPacket.getHeader().getSequenceNumber();
        std::cout << this->window << std::endl;
        // Case 2.1 - if we recieved an eof packet 

        /* If we get the EOF_Packet and it will be the lower part of window*/
        // Case 2.2 - see if the packet is a duplicate packet 
        if(window.getLower() > seqNum){
            std::cout << "lower higher than seqNum" << std::endl;
            RCopyACKPacket &&rr = this->buildACKPacket(this->window.getLower(), RR_PACKET);
            this->sendACKPacket(rr);
        // Case 2.3 - expected packet
        }else if(this->window.getLower() == seqNum){
            /* Case 2.3.1 - if the EOF_PACKET has flag with lower being EOF*/

            std::cout << "lower equal to seqNum" << std::endl;
            this->window.insert(recvPacket);
            return this->fillHoles();
            /* TODO: if out of order packet and corrputpino packet */
        // Case 5 - out of order packet (current is next available packet space)
        }else if (this->window.getLower() < seqNum ){ // really just only to send srej

            std::cout << "lower less to seqNum" << std::endl;
            if(!this->window.inWindow(seqNum)){
                this->window.insert(recvPacket);
            }
            /* give the highest ack sendsrej*/
            for(uint32_t i = this->window.getCurrent(); i < seqNum; i++){
                if(!this->window.inWindow(i)){
                    RCopyACKPacket &&p = this->buildACKPacket(i, SREJ_PACKET);
                    this->window.setCurrent(i+1);
                    this->sendACKPacket(p);
                    break;
                }
            }
        }
    }catch(CorruptPacketException &e){
        return RECV_DATA;
    }
    return RECV_DATA;

}
state_t RCopy::fillHoles(){
    int upper=this->window.getUpper();
    int lower=this->window.getLower();
    uint32_t highestSeqNum = 0;
    for(int i = lower; i <= upper; i++){
        if(!this->window.inWindow(i)){
            break;
        }else{
            RCopyPacket inWind = this->window.getPacket(i);
            this->writePacketToFile(inWind);
            // What if the EOF packet is in the window
            if(inWind.getHeader().getFlag() == EOF_PACKET){
                std::cout << "EOF packet seqNum: " << i << std::endl;
                std::cout << "EOF packet payloadSize" << inWind.getPayloadSize() << std::endl;
                RCopyACKPacket &&eofRR = this->buildACKPacket(i+1, EOF_PACKET_ACK);
                this->sendACKPacket(eofRR);
                return DONE;
            }else{
                highestSeqNum=i;
            }
            this->window.slide(i+1);
        }
       
    }
    /* After we fill we want to send the highest RR we can*/
    if(highestSeqNum !=0){
        RCopyACKPacket &&rr = this->buildACKPacket(highestSeqNum+1, RR_PACKET);
        this->sendACKPacket(rr);
    }
    return RECV_DATA;
}



void RCopy::start(){
    state_t state = FILENAME;
    while(1){
        switch (state)
        {
            case FILENAME:
            {
                static int count=0;
                // open sockets
                // send filename
                if((state = sendFileName()) == FILENAME){
                    safe_close(gateway.getSocketNumber());
                    gateway.setSocketNumber(gateway.setup(args.getRemoteMachine(), 
                                                          args.getPort()));
                    state = count++ > 9 ? DONE: state;
                }
            }
            break;
            case FILENAME_OK:
            {
                // open file
                if((toFile = fopen(args.getToFileName(), "w+")) == NULL){
                    perror("cant open <to-file>");
                    state = DONE;
                }else{
                    state = RECV_DATA;
                }
                std::cout << "in FILENAME_OK"  << std::endl;

            } 
            break;
            case RECV_DATA:
            {
                
                state = receieveData();

            }
            break;
            case DONE:
            {
                safe_fclose(toFile);
                safe_close(this->gateway.getSocketNumber());
                return;
            }
            break;
            default:
                return;
            break;

        }
    }
}
