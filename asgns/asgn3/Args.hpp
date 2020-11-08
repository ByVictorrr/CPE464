#include <iostream>


/***********************rcopy args parser***************************/
#define RCOPY_ARGS 7
// TODO: maybe make this class static (only called once)
class RCopyArgs{
    private:
        const char *fromFileName;
        const char *toFileName;
        int windowSize;
        int bufferSize;
        float errorPercent;
        const char *remoteMachine;
        int port;
    public:
        RCopyArgs(RCopyArgs &args){
            this = args;
        }
        RCopyArgs(const char *fromFileName, const char *toFileName, int ws, int bs, 
                  float erPer, const char *remoteMachine, int port)
                  : fromFileName(fromFileName), toFileName(toFileName), windowSize(ws), bufferSize(bs),
                    errorPercent(erPer), remoteMachine(remoteMachine), port(port){}

        inline const char *getFromFileName(){return this->fromFileName;}
        inline const char *getToFileName(){return this->toFileName;}
        inline int &getWindowSize(){return this->windowSize;}
        inline int &getBufferSize(){return this->bufferSize;}
        inline float &getErrorPercent(){return this->errorPercent;}
        inline const char *getRemoteMachine(){return this->remoteMachine;}
        inline int &getPort(){return this->port;}
};

class RCopyArgsParser{
    private:
    public:
        static RCopyArgs parse(int argc, char *argv[]){
            const char *toFileName, *fromFileName, *remoteMachine;
            int windowSize, bufferSize, port;
            float errorPer;
            // Step 1 - check to see if right amout of args
            if (argc != RCOPY_ARGS+1){
                std::cout << std::string(argv[0]) + " <from-filename> <to-filename> <window-size> <buffer-size> <error-percent> <remote-machine> <remote-port>" << std::endl;
                exit(EXIT_FAILURE);
            }
            // step 2 - create RcopyArgs obj
            fromFileName = argv[1];
            toFileName = argv[2];
            windowSize = std::atoi(argv[3]);
            bufferSize = std::atoi(argv[4]);
            errorPer = std::atof(argv[5]);
            remoteMachine = argv[6];
            port = std::atoi(argv[7]);
            // Step 1 - check buffer size
            if(bufferSize < 1 || bufferSize > MAX_PAYLOAD_LEN){
                std::cout << "bufferSize has to be between 1 - 1400 inclusive" << std::endl;
                exit(EXIT_FAILURE);
            }
            // Step 2 - check error percent
            if(errorPer <= 0 || errorPer >= 1){
                std::cout << "errorPercent has to be between 0 - 1 exclusive" << std::endl;
                exit(EXIT_FAILURE);
            }
            // TODO : max window size?
 
            return RCopyArgs(fromFileName, toFileName, windowSize, bufferSize, errorPer, remoteMachine, port);
        }
};

/***********************Server args parser***************************/
class ServerArgs{
    private:
        float errorPercent;
        int portNumber;
    public:
        ServerArgs(float erPer, int portNum){
            this->errorPercent = erPer;
            this->portNumber = portNum;
        }
        inline float getErrorPercent(){return this->errorPercent;}
        inline float getPortNumber(){return this->portNumber;}

};

class ServerArgsParser{
    private:
    public:
        static ServerArgs parse(int argc, char *argv[]){
            float errorPer;
            int port;
            if(argc != 2+1 || argc != 3+1){
                std::cout << std::string(argv[0]) + " <error-percent> [port-number]" << std::endl;
                exit(EXIT_FAILURE);
            }
            errorPer = std::atof(argv[1]);
            if(argc==2+1){
                port=0;
            }else{
                port=std::atoi(argv[2]);
            }
            return ServerArgs(errorPer, port);
        }
};