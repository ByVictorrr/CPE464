#include <regex>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>

static const char *COMMAND_FORMAT = "^\\s{0,}%([M|m]\\s{1,}[1-9](\\s{1,}[^\\s]{1,100}){1,9}?(\\s{1,}[^\\s]{1,}){0,}";
   
    void parse(std::string &input)throw (const char *){
        uint8_t numHandles = parseNumHandles(input);
        std::queue<std::string> messages = parseMessages(input, numHandles);
        std::vector<std::string> destHandles = parseDestHandles(input, numHandles);
        int var = 1 ;
    }

int main()
{
    std::string s = "%M  	1	handle mesage1kdafdf adfdfadffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
    std::string s1 = "   %M  	3	handle1 mesage1kdafdf adfdf handle2";
    std::string s2 = "   % M  	10 9	    handle mesage1kdafdf adfdf";
    std::string s3 = "   % M  	1 9	    handle mesage1kdafdf adfdf";
    /*
    std::regex rgx("\\s{0,}%M\\s{0,}([1-9])");
    std::smatch match;

    if (std::regex_search(s.begin(), s.end(), match, rgx))
        std::cout << "match: " << match[1] << '\n';
        */

    std::queue<std::string> spi = split(s, 200);
try{
        parse(s);
       std::cout << "--------------------" << std::endl;
       parse(s1);
       std::cout << "--------------------" << std::endl;
       parse(s2);
       std::cout << "--------------------" << std::endl;
}catch(const char *e){
    std::cout << e << std::endl;
}


}