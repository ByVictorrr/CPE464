#include "Args.hpp"
#include <string.h>
RCopyArgs RCopyArgsParser::parse(int argc, char *argv[]){
    const char *toFileName, *fromFileName, *remoteMachine;
    int windowSize, bufferSize, port;
    float errorPer;
    // Step 1 - check to see if right amout of args
    if (argc != RCOPY_ARGS+1){
        std::cerr << std::string(argv[0]) + " <from-filename> <to-filename> <window-size> <buffer-size> <error-percent> <remote-machine> <remote-port>" << std::endl;
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
    if(bufferSize < 1 || bufferSize > 1400){
        std::cerr << "BufferSize has to be between 1 - 1400 inclusive" << std::endl;
        exit(EXIT_FAILURE);
    }
    // Step 2 - check error percent
    if(errorPer < 0 || errorPer > 1){
        std::cerr << "ErrorPercent has to be between 0 - 1 inclusive" << std::endl;
        exit(EXIT_FAILURE);
    }
    // step 3 - check filelen
    if(strlen(fromFileName)+1 > 100){
        std::cerr << std::string(fromFileName) << "has to be less than 1400 chars(including NULL)" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if(strlen(toFileName)+1 > 100){
        std::cerr << std::string(fromFileName) << "has to be less than 1400 chars(including NULL)" << std::endl;
        exit(EXIT_FAILURE);
    }

    return RCopyArgs(fromFileName, toFileName, windowSize, bufferSize, errorPer, remoteMachine, port);
}


ServerArgs ServerArgsParser::parse(int argc, char *argv[]){
    // Step 1 - check to see if right amout of args
    float errorPer;
    int port;
    if(argc != 2 && argc != 3){
        std::cout << std::string(argv[0]) + " <error-percent> [port-number]" << std::endl;
        exit(EXIT_FAILURE);
    }
    errorPer = std::atof(argv[1]);
    if(argc==2){
        port=0;
    }else{
        port=std::atoi(argv[2]);
    }
    return ServerArgs(errorPer, port);
}