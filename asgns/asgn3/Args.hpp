#ifndef ARGS_H_
#define ARGS_H_
#include <iostream>

/***********************rcopy args parser***************************/
#define RCOPY_ARGS 7
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
        RCopyArgs(){}

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
        RCopyArgsParser(){}
    public:
        static RCopyArgs parse(int argc, char *argv[]);
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
        ServerArgsParser(){}
    public:
        static ServerArgs parse(int argc, char *argv[]);
};
#endif