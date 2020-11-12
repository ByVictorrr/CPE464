#include <thread>
#include <string>
#include <list>
#include "networks.hpp"
#include "Packet.hpp"
#include "safe_sys_calls.h"
#include "Utils.hpp"

class Server{
    private:
        ServerConnection gateway;
        std::list<ServerThread> clients;
        ServerArgs args;

    public:
        Server(ServerArgs &args)
        : gateway(args.getPortNumber()), args(args){}

        void serve(){
            start:
                // Step 1 - see if a new client is trying to connect
                if(safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0) == true){
                    // create thread 
                    try{
                        RCopySetupPacket setup = RCopyPacketReciever::RecieveSetup(this->gateway);
                        ServerThread t(setup, args);
                        t.join();
                    }catch(CorruptPacketException &e){
                        ; // kill thread
                    }
                }

            goto start;

        }



};

typedef enum STATES{FILENAME, SEND_DATA, WAITING, RECV_DATA, DONE} state_t;

class ServerThread{
    private:
        std::thread thread;
        FILE *file;
        uint32_t bufferSize;
        Window *window;
        ServerArgs args;
        ServerConnection gateway;
        ssize_t sendPacket(RCopyPacket &packet){
            return RCopyPacketSender::Send(this->gateway.getSocketNumber(), 
                                           packet, 
                                           this->gateway);
        }

        

        
    public:
        ServerThread(RCopySetupPacket &setup, ServerArgs &args)
        : thread(processRCopy, setup), args(args), gateway(args.getPortNumber()){}
        
        state_t sendData(RCopyPacket *recvd){

            if(this->window->isClosed()){
                return WAITING;
            }else{                    

                RCopyPacket p = this->window->getPacket(this->window->getLower());
                sendPacket(p);
                this->window->setCurrent(this->window->getLower());
                if(safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0) ){                        
                    try{
                        *recvd = RCopyPacketReciever::Recieve(this->bufferSize, 
                                                              this->gateway);
                    }catch(CorruptPacketException &e){
                        return SEND_DATA;
                    }
                     // if something waiting 
                    return RECV_DATA;
                }
            }
        }
        state_t waiting(RCopyPacket *recvd){

            int count = 0;
            start:
            if(safeSelectTimeout(this->gateway.getSocketNumber(), 1, 0)){
                // recieved message
                try{
                    *recvd = RCopyPacketReciever::Recieve(this->bufferSize, this->gateway);
                }catch(CorruptPacketException &e){
                    return WAITING;
                }
                return RECV_DATA;
            }else{
                //timeout
                if(count > 9){
                   return DONE; 
                }else{
                    // TODO: look for lowest unacked packet
                    RCopyPacket p = this->window->getPacket(this->window->getLower());
                    sendPacket(p);
                    count++;
                    return WAITING;
                }

            }
            goto start;
        }
        state_t receiveData(RCopyPacket &recvd){
            flag_t flag;

            if((flag=(flag_t)recvd.getHeader().getFlag()) == SREJ_PACKET){
                RCopyPacket p = this->window->getPacket(recvd.getHeader().getSequenceNumber());
                sendPacket(p);
            }else if(flag == RR_PACKET){
                this->window->slide(recvd.getHeader().getSequenceNumber()+1);
            }else if(flag == EOF_PACKET_ACK){
                return DONE;
            }
            return SEND_DATA;
        }
        void processRCopy(RCopySetupPacket &setup)
        {
            char *fileName;
            state_t state = FILENAME;


            // step 1 - parse setup packet
            fileName = setup.getFileName();
            bufferSize = setup.getBufferSize();
            this->window = new Window(setup.getWindowSize());
            RCopyPacket recvd;
            // Step 2 - go through the process
            while(1){
                switch (state)
                {
                    case FILENAME:
                    {
                        if((file = safe_fopen(fileName, "r+")) == NULL){
                            RCopyPacket p = RCopyPacketBuilder::Build(0, FILENAME_PACKET_BAD, NULL, bufferSize);
                            // send bad filename 
                            sendPacket(p);
                            state = DONE;
                        }else{
                            RCopyPacket p = RCopyPacketBuilder::Build(0, FILENAME_PACKET_OK, NULL, bufferSize);
                            sendPacket(p);
                            state=SEND_DATA;
                        }

                    }
                    break;
                    case SEND_DATA:
                    {

                        // send first data packet

                        state = sendData(&recvd);

                    }
                    break;
                    case WAITING:
                    {
                        state = waiting(&recvd);
                    }
                    break;
                    case RECV_DATA:
                    {
                        state = receiveData(recvd);

                    }
                    break;
                    case DONE:
                    {
                        safe_close(gateway.getSocketNumber());
                        safe_fclose(file);
                        return;
                    }
                    break;
                    
                    default:
                        break;
                    }
            }
            
        }

        void join(){thread.join();}



};