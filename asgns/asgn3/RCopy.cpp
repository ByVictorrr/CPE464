#include "RCopy.hpp"
/*
#include "Packet.hpp"
#include "networks.hpp"

class RCopyPacketBuilder;
class RCopyPacketSender;
class RCopyPacketSender;
class RCopyPacket;
class RCopySetupPacket;
*/
/********* Constructor/destructor***********/
RCopy::RCopy(RCopyArgs & cmdArgs)
: args(cmdArgs), window(cmdArgs.getWindowSize()), 
  gateway(args.getRemoteMachine(), args.getPort()){}



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
        RCopyPacketReciever::Recieve(this->args.getBufferSize(), this->gateway);
    }catch(CorruptPacketException &e){
        throw e;
    }
}

RCopyPacket RCopy::buildRR(uint32_t seqNum){
    return RCopyPacketBuilder::Build(seqNum, RR_PACKET, NULL, args.getBufferSize());
}
RCopyPacket RCopy::buildSEJ(uint32_t seqNum){
    return RCopyPacketBuilder::Build(seqNum, SREJ_PACKET, NULL, args.getBufferSize());
}

size_t RCopy::writePacketToFile(RCopyPacket &p){
    size_t len;
    if((len=fwrite((void*)p.getPayload(), 
                    (size_t)sizeof(uint8_t),
                    p.getPayloadSize(), 
                    this->toFile)) != p.getPayloadSize()){
                        std::cerr << "problem writing " << std::endl;
                        return 0;
                    }
        return len;
}



//============= State functions ===============//
state_t RCopy::sendFileName(){
    uint8_t flag;
    state_t ret;
    RCopySetupPacket builtPacket = RCopyPacketBuilder::BuildSetup( 
                                                            (uint32_t)args.getBufferSize(), 
                                                            (uint32_t)args.getWindowSize(), 
                                                            args.getFromFileName()
                                                            );
    RCopyPacket recievedPacket;
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
            }else{
                ret = FILENAME;
            }
        }catch(CorruptPacketException &e){
            ret = FILENAME;
        }
    }

    return ret;
}

state_t RCopy::recvData(){
    state_t ret;
    uint8_t flag;
    uint32_t seqNum;
    
    // Step 1 - get the first packet
    if(safeSelectTimeout(this->gateway.getSocketNumber(), 10, 0) == false)
        return DONE;

    try{
        RCopyPacket recvPacket = recievePacket();
        seqNum = recvPacket.getHeader().getSequenceNumber();

        // Case 1 - if the packet is eof
        if((flag=recvPacket.getHeader().getFlag()) == (uint8_t)EOF_PACKET){
            RCopyPacket &&rr = buildRR(seqNum);
            sendPacket(rr);
            return DONE;
        }
        // Case 2 - see if the packet is a duplicate packet 
        if(window.getLower() < seqNum){
            RCopyPacket &&rr = buildRR(seqNum+1);
            sendPacket(rr);
            return RECV_DATA;
        // Case 3 - out of window packet 
        }else if(window.getUpper() < seqNum){
            std::cout << "out of window" << std::endl;
            return RECV_DATA;
        // Case 4 - expected packet
        }else if(window.getLower() == seqNum){
            // write to disk and slide window
            window.slide(seqNum+1);
            writePacketToFile(recvPacket);
            return RECV_DATA;
        // Case 5 - out of order packet
        }else{
            window.insert(recvPacket);
            return RECV_DATA;
        }
    }catch(CorruptPacketException &e){
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
                if((toFile = fopen(args.getToFileName(), "w+"))){
                    perror("cant open <to-file>");
                    state = DONE;
                }else{
                    state = RECV_DATA;
                }

            } 
            break;
            case RECV_DATA:
            {
                
                state = recvData();
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
