#include "Packet.hpp"
#include "Args.hpp"
#include "Utils.hpp"
#include "networks.hpp"
#include "Exception.hpp"
#include "safe_sys_calls.h"

typedef enum RCOPY_STATES{FILENAME, FILENAME_OK, RECV_DATA, DONE} state_t;


class RCopy{
    private:
       Window window;
       RCopyConnection gateway;
       RCopyArgs args;
       int socket;
       FILE *toFile;
       // ================Two main functions===========================//
        ssize_t sendPacket(RCopyPacket &packet){
           return RCopyPacketSender::Send(this->socket, packet, this->gateway);
        }
        RCopyPacket recievePacket() throw (CorruptPacketException){
           try{
               // Step 1 - recv the packet
               RCopyPacketReciever::Recieve(this->socket, this->args.getBufferSize(), this->gateway);
               // Step 2 - put in the window

           }catch(CorruptPacketException &e){
               throw e;
           }
        }
        RCopyPacket buildRR(uint32_t seqNum){
            return RCopyPacketBuilder::Build(seqNum, RR_PACKET, NULL, args.getBufferSize());
        }
        RCopyPacket buildSEJ(uint32_t seqNum){
            return RCopyPacketBuilder::Build(seqNum, SREJ_PACKET, NULL, args.getBufferSize());
        }

        size_t writePacketToFile(RCopyPacket &p){
            size_t len;
            if((len=fwrite((void*)p.getPayload(), 
                            (size_t)sizeof(uint8_t),
                            p.getPayloadLen(), 
                            this->toFile)) != p.getPayloadLen()){
                                std::cerr << "problem writing " << std::endl;
                                return NULL;
                            }
                return len;
        }



    //============= State functions ===============//
       state_t sendFileName(){
           uint8_t flag;
           state_t ret;
           RCopyPacket &&builtPacket = RCopyPacketBuilder::BuildSetupPacket(0, 
                                                                 FILENAME_PACKET, 
                                                                 args.getBufferSize(), 
                                                                 args.getWindowSize(), 
                                                                 args.getFromFileName()
                                                                 );
           RCopyPacket recievedPacket;
           this->sendPacket(builtPacket);
           if(safeSelectTimeout(socket, 1, 0)){
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

        state_t recvData(){
            state_t ret;
            uint8_t flag;
            uint32_t seqNum;
            
            // Step 1 - get the first packet
            if(safeSelectTimeout(this->socket, 10, 0) == false)
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
                    writePacketToFile(recievePacket);
                    return RECV_DATA;
                // Case 5 - out of order packet
                }else{
                    window.insert(recievePacket);
                    return RECV_DATA;
                }
            }catch(CorruptPacketException &e){
                return RECV_DATA;
            }
        }


    public:
        RCopy(RCopyArgs & cmdArgs)
        : args(cmdArgs), window(cmdArgs.getWindowSize()), 
          gateway(args.getRemoteMachine(), args.getPort()){}

        void start(){

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
                            safe_close(*gateway.getSocketNumber());
                            gateway.setSocketNumber(gateway.setup(gateway.getRemote(), args.getRemoteMachine(), args.getPort()));
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
                        safe_close(this->socket);
                        return;
                    }
                    break;
                    default:


                }
            }
       }
       
  };