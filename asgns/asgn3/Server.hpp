#include <thread>
#include <string>
#include <list>
#include "networks.hpp"
#include "Packet.hpp"

class Server{
    private:
        int socketNum;
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
	            select_call(socketNum, 1, 0 , 0);
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

template<typename T>
class CircularQueue{
    private:
        int front, rear;
        int size;
        T *arr;
    public:
        CircularQueue(int size){
            this->front = -1;
            this->rear = -1;
            this->size = size;
            this->arr = new T[size];
        }
        T getFront(){
            return arr[front];
        }
        T getRear(){
            return arr[rear];
        }
        /* Function to create Circular queue */
        void enQueue(T value) 
        { 
            if ((front == 0 && rear == size-1) || 
                    (rear == (front-1)%(size-1))) 
            { 
                printf("\nQueue is Full"); 
                return; 
            } 
        
            else if (front == -1) /* Insert First Element */
            { 
                front = rear = 0; 
                arr[rear] = value; 
            } 
        
            else if (rear == size-1 && front != 0) 
            { 
                rear = 0; 
                arr[rear] = value; 
            } 
        
            else
            { 
                rear++; 
                arr[rear] = value; 
            } 
        } 
        
        // Function to delete element from Circular Queue 
        T deQueue() 
        { 
            if (front == -1) 
            { 
                printf("\nQueue is Empty"); 
                return INT_MIN;  // todo
            } 
        
            T data = arr[front]; 
            arr[front] = -1; 
            if (front == rear) 
            { 
                front = -1; 
                rear = -1; 
            } 
            else if (front == size-1) 
                front = 0; 
            else
                front++; 
        
            return data; 
        } 
};
class SelectiveReject{
    private:
        CircularQueue <RCopyPacket> window;
    public:
        SelectiveReject(int windowSize)
            : window(windowSize){}
        
        CircularQueue <RCopyPacket> &getWindow(){
            return this->window;
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