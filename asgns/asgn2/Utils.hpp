#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <sys/socket.h>
#include "safe_sys_calls.h"
#include <netinet/in.h>
#include "gethostbyname6.h"
#include <vector>
#include <sstream>
#include <regex>

#define HDR_LEN 2
#define FLAG_LEN 1
#define MAX_BUFF (1 << (HDR_LEN*8)) // TODO : ask if i can use bit shift in macro
#define BACKLOG 10


typedef enum FLAGS{
    NOT_USED,
    CHECK_HANDLE,
    HANDLE_DNE,
    HANDLE_EXISTS,
    BROADCAST,
    MULTICAST,
    NOT_USED,
    MULTICAST_HANDLE_DNE,
    CLIENT_EXIT,
    CLIENT_EXIT_ACK,
    LIST_HANDLES,
    NUM_HANDLES,
    LIST_HANDLE_NAME,
    FINISHED_LIST_HANDLES
} flags_t;


std::string trim_left(const std::string& str){
    const std::string pattern = " \f\n\r\t\v";
    return str.substr(str.find_first_not_of(pattern));
}
std::string trim_right(const std::string& str){
    const std::string pattern = " \f\n\r\t\v";
    return str.substr(0,str.find_last_not_of(pattern) + 1);
}
std::string trim(const std::string& str){
    return trim_left(trim_right(str));
}


std::vector<std::string> splitByWhiteSpace(std::string &input){
    std::vector<std::string> result; 
    std::istringstream iss(input); 
    for(std::string s; iss >> s; ) 
        result.push_back(s); 
    return result;
}


std::queue<std::string> split(const std::string str, int splitLen){
    std::queue<std::string> ret;
    for (size_t i = 0; i < str.size(); i += splitLen)
        ret.push(str.substr(i, splitLen));
    return ret;
}
int getFirstDigit(std::string &str)throw(const char *){
    for(auto c: str){
        if(std::isdigit(c)){
            return 48 - c;
        }
    }
    throw "No digit in this string";
}

#endif
