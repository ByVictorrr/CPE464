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


    public:
        RCopy(RCopyArgs & cmdArgs)
        : args(cmdArgs), window(cmdArgs.getWindowSize()), 
          gateway(args.getRemoteMachine(), args.getPort()){}

        void start(){

            state_t state = FILENAME;
            while(state != DONE){
                switch (state)
                {
                    case FILENAME:
                    {
                        static int count=0;
                        // open sockets
                        gateway.setSocketNumber(gateway.setup(gateway.getRemote(), args.getRemoteMachine(), args.getPort()));
                        // send filename
                        if((state = sendFileName()) == FILENAME){
                            safe_close(*gateway.getSocketNumber());
                            count++;
                        }
                        if(count > 9)
                            state = DONE;
                    }
                    break;
                    case FILENAME_OK:
                    {
                        static int count = 0;
                        FILE *toFile;
                        // open file
                        // change state to recv data
                        if((toFile = fopen(args.getToFileName(), "w+"))){
                            perror("cant open to-file");
                            state = DONE;
                        }else{
                            state = RECV_DATA;
                        }

                    } 
                    break;
                    case RECV_DATA:
                    {
                        state = recv_data();
                    }
                    break;
                    case DONE:
                    {

                    }
                    break;
                    default:


                }
            }
       }
       
  };