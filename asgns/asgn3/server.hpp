#include <thread>
#include <string>
#include <list>
#include "networks.hpp"
#include "Packet.hpp"
#include "safe_sys_calls.h"

class Server{
    private:
        ServerConnection gateway;
        std::list<ServerThread> clients;
        int portNumber;
        float errPercent;
    public:
        Server(int portNumber, float errPercent){
            this->portNumber = portNumber;
            this->errPercent = errPercent;
        }
        void connect(){
           this->socketNum = udpServerSetup(this->portNumber);
        }
        void serve(){
            start:
                // Step 1 - see if a new client is trying to connect
                if(safeSelectTimeout(this->socketNum, 1, 0) == true){
                    
                }
                safe_select(this->socketNum+1, 1, NULL, NULL, )
                // Step 2 - wait and read that packet 
                // getFileName(windowSize, buffSize, fileName) 
                // Step 3 - pass those values to the client threads
	            clients.push(std::thread(processClient, socketNum, errPercent));

            goto start;

        }
        void disconnect(){
            close(this->socketNum);
        }



};


class ServerThread{
    private:
        std::thread &&thread;
        std::string fileName;
        int windowSize;
        int bufferSize;
        SelectiveReject flowControl;


        void processClient(int initalSocketNum, float errPercent, 
                           char *filename, int windowSize, int bufferSize)
        {

            int dataLen = 0;
            struct sockaddr_in6 client;
            int clientAddrLen = sizeof(client);

            RCopyPacketParser parser;
            sendtoErr_init(errPercent, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);


            // Step 1 - try to open it

            flowControl.getWindow().enQueue();
            while (buffer[0] != '.')
            {


                memset(buffer, 0, MAXBUF);
                printIPInfo(&client);

            }
        }
        
    public:
        ServerThread(std::string fileName, int windowSize, int bufferSize){
            this->fileName = fileName;
            this->windowSize = windowSize;
            this->bufferSize = bufferSize;
            this->flowControl = SelectiveReject(windowSize);

        }
        void join(){
            thread.join();
        }

};