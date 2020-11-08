#include "Packet.hpp"
#include "Args.hpp"
#include "Utils.hpp"
#include "networks.hpp"

typedef enum RCOPY_STATES{FILENAME, FILENAME_OK, RECV_DATA, DONE} state_t;


class RCopy{
    private:
       CircularQueue<RCopyPacket> window;
       RCopyConnection server;
       RCopyArgs args;
       int socket;
       // Helper functions
       ssize_t sendPacket(RCopyPacket &packet){
            ssize_t sendLen;
            if((sendLen = sendtoErr(this->socket, packet.getRawPacket(), packet.getPacketLen(), 0, 
                        (struct sockaddr *)this->server.getRemote(), sizeof(*this->server.getRemote()))) < 0){
                    perror("ERROR from sendtoErr");
                    exit(EXIT_FAILURE);
            }
            return sendLen;
       }
       RCopyPacket recievePacket(){
           uint8_t temp[MAX_PAYLOAD_LEN+HDR_LEN];
           socklen_t remoteLen = sizeof(*server.getRemote());
           ssize_t recvLen;
           memset(temp, 0, MAX_PAYLOAD_LEN+HDR_LEN);
           if((recvLen = recvfromErr(this->socket, temp, args.getBufferSize()+HDR_LEN, 0, 
                              (struct sockaddr *)server.getRemote(), &remoteLen)) < 0){
                perror("ERROR from recievePacket");
                exit(EXIT_FAILURE);
           }
           // check crc in RCopyPacket Parseer
           return RCopyPacketParser::parsePacket(temp, args.getBufferSize());
       }

       state_t sendFileName(){

           RCopyPacket &&builtPacket = RCopyPacketBuilder::buildFileNameRequestPacket(0, FILENAME_PACKET, args.getBufferSize(), 
                                                         args.getWindowSize(), args.getFromFileName());
           sendPacket(builtPacket);

           if(select(socket, 1, 0, 0) == 1){
               // read the packet and check crc
               try{
                    RCopyPacket recv = recievePacket();
               }catch(const char *){
                    // crc return filename
                    // file not found return DONE
               }

               return FILENAME_OK;
           }
           return FILENAME;
       }


    public:
        RCopy(RCopyArgs & cmdArgs)
        :   args(cmdArgs), window(cmdArgs.getWindowSize()), server(args)
        {
        }

        void start(){

            state_t state = FILENAME;
            while(1){
                switch (state)
                {
                    case FILENAME:
                    {
                        static int count=0;
                        // open sockets
                        server.setSocketNumber(server.setupConnection(server.getRemote(), args.getRemoteMachine(), args.getPort()));
                        // send filename
                        if((state = sendFileName()) == FILENAME){
                            close(server.getSocketNumber());
                        }

                        if(count++ > 9)
                            state = DONE;

                    }
                    break;
                    case FILENAME_OK:
                    {

                    } 
                    break;
                    case RECV_DATA:
                    {

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