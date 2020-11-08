#include "Packet.hpp"
#include "Args.hpp"
#include "Utils.hpp"
#include "networks.hpp"

typedef enum RCOPY_STATES{FILENAME, FILENAME_OK, RECV_DATA, DONE} state_t;

class RCopy{
    private:
       CircularQueue<RCopyPacket> window;
       RCopyConnection &&server;
       RCopyArgs args;
       int socket;
       // Helper functions
       state_t sendFileName(){
           return FILENAME_OK;
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
                        server.setupConnection(server.getRemote(), args.getRemoteMachine(), args.getPort());
                        // send filename
                        state = sendFileName();
                        // count = 0
                        // select(1)
                        //if got response
                            // is response good?
                                //state=FILENAME_OK
                            // is response bad?
                                //state = DONE

                        if(count > 9)
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