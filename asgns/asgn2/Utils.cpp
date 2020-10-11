#include "Utils.hpp"

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
            return c - '0';
        }
    }
    throw "No digit in this string";
}
