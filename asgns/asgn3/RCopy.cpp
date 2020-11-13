#include "RCopy.hpp"
/********* Constructor/destructor***********/
RCopy::RCopy(RCopyArgs & cmdArgs)
: args(cmdArgs), window(cmdArgs.getWindowSize()), 
  gateway(args.getRemoteMachine(), args.getPort()), toFile(NULL){}



ssize_t RCopy::sendPacket(RCopyPacket &packet){
    return RCopyPacketSender::Send(packet, this->gateway);
}
ssize_t RCopy::sendSetupPacket(RCopySetupPacket &p){
    return RCopyPacketSender::SendSetup(p, this->gateway);
}

/**
 * 
 */
RCopyPacket RCopy::recievePacket() throw (CorruptPacketException){
    try{
        return RCopyPacketReciever::Recieve(this->args.getBufferSize(), this->gateway);
    }catch(CorruptPacketException &e){
        throw e;
    }
}

RCopyPacket RCopy::buildPacket(uint32_t seqNum, uint8_t flag){
    return RCopyPacketBuilder::Build(seqNum, flag, NULL, args.getBufferSize());
}


size_t RCopy::writePacketToFile(RCopyPacket &p){
    size_t len;
    if((len=fwrite((void*)p.getPayload(), (size_t)sizeof(uint8_t), p.getPayloadSize(), 
                   this->toFile)) != p.getPayloadSize()){
                        std::cerr << "problem writing " << std::endl;
                        return 0;
                    }
        std::cout << "wrote " << p.getPayloadSize() << std::endl; 
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
    std::cout << builtPacket << std::endl; // debug
    if(safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0)){
        // read the packet and check crc
        try{
            recievedPacket = recievePacket();
            RCopyPacketDebugger::println(recievedPacket);
            if((flag=recievedPacket.getHeader().getFlag()) == FILENAME_PACKET_BAD){
                std::cerr << "File " + std::string(args.getFromFileName()) + "Not found" << std::endl;
                ret = DONE;
            }else if(flag == FILENAME_PACKET_OK){
                ret = FILENAME_OK;
            }
        }catch(CorruptPacketException &e){
            ret = FILENAME;
        }
    }

    return ret;
}

state_t RCopy::receieveData(){
    state_t ret;
    uint8_t flag;
    uint32_t seqNum;
    
    // Case 1 - no data packet waiting
    if(!safeSelectTimeout(this->gateway.getSocketNumber(), 10, 0))
        return DONE;

    // Case 2 - data packet waiting
    try{
        RCopyPacket &&recvPacket = recievePacket();
        seqNum = recvPacket.getHeader().getSequenceNumber();
        flag = recvPacket.getHeader().getFlag();

        // Case 2.1 - if we recieved an eof packet 
        if(flag == EOF_PACKET){
            RCopyPacket &&eof = this->buildPacket(seqNum, EOF_PACKET_ACK);
            fflush(toFile);
            std::cout << "IN EOF" << eof.size() << std::endl;
            this->sendPacket(eof);
            this->writePacketToFile(recvPacket);
            return DONE;
        }

        // Case 2.2 - see if the packet is a duplicate packet 
        if(window.getLower() > seqNum){
            RCopyPacket &&rr = this->buildPacket(seqNum, RR_PACKET);
            this->sendPacket(rr);
            return RECV_DATA;
        // Case 2.3 - expected packet
        }else if(window.getLower() == seqNum){
            // write to disk and slide window
            RCopyPacket &&rr = this->buildPacket(seqNum, RR_PACKET);
            this->sendPacket(rr);
            this->window.insert(recvPacket);
            // check if adjacent packet are filled (fill holes)
            for(int i = seqNum; i <= this->window.getUpper(); i++){
                if(!this->window.inWindow(i))
                    break;
                RCopyPacket &inwWind = this->window.getPacket(i);
                this->writePacketToFile(inwWind);

                fflush(toFile);
                this->window.slide(i+1);
            }
            return RECV_DATA;

        /* TODO: if out of order packet and corrputpino packet */

        // Case 5 - out of order packet (current is next available packet space)
        }else if (this->window.getLower() < seqNum){ // really just only to send srej
            for(int i = this->window.getLower(); i < seqNum; i++){
                // holes
                if(!this->window.inWindow(i)){
                    RCopyPacket &&p = this->buildPacket(i, SREJ_PACKET);
                    this->sendPacket(p);
                }
            }
            this->window.insert(recvPacket);
            return RECV_DATA;

        }
    }catch(CorruptPacketException &e){
        
        RCopyPacket &&srej = this->buildPacket(seqNum, SREJ_PACKET);
        this->sendPacket(srej);
        return RECV_DATA;
    }

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
                    gateway.setSocketNumber(gateway.setup(gateway.getRemote(), 
                                                          args.getRemoteMachine(), 
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
