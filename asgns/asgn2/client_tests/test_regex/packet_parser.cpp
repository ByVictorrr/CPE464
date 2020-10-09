#include <regex>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>

typedef enum FLAGS{
    NOT_USED,
    CHECK_HANDLE,
    HANDLE_DNE,
    HANDLE_EXISTS,
    BROADCAST,
    MULTICAST,
    UN_USED,
    MESSAGE_HANDLE_DNE,
    CLIENT_EXIT,
    CLIENT_EXIT_ACK,
    LIST_HANDLES,
    NUM_HANDLES,
    LIST_HANDLE_NAME,
    FINISHED_LIST_HANDLES
} flags_t;

class CommandParser{
     
    protected:
        char cmd;
        static std::vector<std::string> splitByWhiteSpace(std::string &input){
            std::vector<std::string> result; 
            std::istringstream iss(input); 
            for(std::string s; iss >> s; ) 
                result.push_back(s); 
            return result;
        }

        static std::string trim_left(const std::string& str){
            const std::string pattern = " \f\n\r\t\v";
            return str.substr(str.find_first_not_of(pattern));
        }
        static std::string trim_right(const std::string& str){
            const std::string pattern = " \f\n\r\t\v";
            return str.substr(0,str.find_last_not_of(pattern) + 1);
        }
        static std::queue<std::string> split(const std::string str, int splitLen){
            std::queue<std::string> ret;
            for (size_t i = 0; i < str.size(); i += splitLen)
                ret.push(str.substr(i, splitLen));
            return ret;
        }
    public:
         static std::string trim(const std::string& str){
            return trim_left(trim_right(str));
         }
        static char getCommand(std::string &input){
           std::string tr = trim(input);
           return std::toupper(tr[1]);
        }
        virtual void parse(std::string &input) = 0;
};


class MCommandParser: public CommandParser{

    private:
        /**
         * Variables of McommandParser
         */
        std::queue<std::string> messages;
        uint8_t numHandles;// only 1-9
        std::vector<std::string> destHandles;
        /**
         * Helper functions for main parse function
         */
        uint8_t parseNumHandles(std::string &input)throw(const char *){
            uint8_t numHandles;
            std::regex rgx("\\s{0,}%[M|m]\\s{0,}([1-9])");
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                std::string handles = match[1];
                numHandles = std::stoi(handles.c_str());
                return numHandles;
            }
            throw "parseNumHandles: Couldn't find the number of handles";
        }
        std::vector<std::string> parseDestHandles(std::string &input, uint8_t handles) throw(const char*){
            std::string format = "\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){"+std::to_string(handles)+"})";
            std::vector<std::string> ret;
            std::regex rgx(format);
            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                std::string handles = trim_left(match[1]);
                ret = splitByWhiteSpace(handles);
                return ret;
            }
        throw "parseDestHandles: The digit 1-9 has to match the number of handles given";
    }



    std::queue<std::string> parseMessages(std::string &input, uint8_t handles){

            std::string format = "\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){"+std::to_string(handles)+"})(.*)";
            std::queue<std::string> ret;
            std::regex rgx(format);

            std::smatch match;
            if (std::regex_search(input, match, rgx)){
                std::string message = trim_left(match[3]);
                ret = split(message, 200);
                return ret;
            }
            // There is no message
            ret.push("\n");
        return ret;
    }

    public:

        // main function
        void parse(std::string &input)throw (const char *){
            this->numHandles = parseNumHandles(input);
            this->destHandles = parseDestHandles(input, this->numHandles);
            this->messages = parseMessages(input, this->numHandles);
        }
        // Getters
        
        std::queue<std::string> &getMessages(){return this->messages;};
        uint8_t &getNumHandles(){return this->numHandles;}
        std::vector<std::string> &getDestHandles(){return this->destHandles;}
};

#define MAX_BUFF 1024
#define HDR_LEN 2
#define FLAG_LEN 1
  /* Format <hdr len (2) | 1 byte flag | 1 byte src-han-len | src-han | dest-han1-len | dest-han1 | ... | msg */
        static uint8_t *buildMPacket(MCommandParser &cmd){
            uint8_t flag = MULTICAST;
            static uint8_t transBuff[MAX_BUFF];
            char *srcHand = "handle";
            uint8_t src_handle_len = std::strlen(srcHand);
            int cursor = 0;
            memset(transBuff,0 ,MAX_BUFF);
            /** Get pkt_size first */
            uint16_t pkt_len = HDR_LEN+FLAG_LEN+1;
            // Step 1 - src-handle length
            pkt_len+=(src_handle_len+1);
            // Step 2- - all destinations-handles length 
            for(int i = 0; i < cmd.getDestHandles().size(); i++){
                pkt_len+=(1+cmd.getDestHandles()[i].size());
            }
            pkt_len += cmd.getMessages().front().size()+1; // +1 for including the null char
            /** Done getting the pkt_size */

            /** Time to fill the trans buff */
            memcpy(transBuff, &pkt_len, 2);
            cursor=2;
            memcpy(transBuff+cursor, &flag, 1);
            cursor++;
            transBuff[cursor++] = src_handle_len;
            memcpy(transBuff+cursor, srcHand, src_handle_len);
            cursor+=src_handle_len;
            memcpy(transBuff+cursor, &cmd.getNumHandles(), 1);
            cursor++;
            for(std::string &destHand: cmd.getDestHandles()){
                transBuff[cursor++] = destHand.size();
                memcpy(transBuff+cursor, destHand.c_str(), destHand.size());
                cursor+=destHand.size();
            }
            // set message
            memcpy(transBuff+cursor, cmd.getMessages().front().c_str(), cmd.getMessages().front().size());
            return transBuff;
        }
static bool MultiCastParse(uint16_t data_len, uint8_t *data){
            // Step 1 - step 1 get src handle
            uint8_t srcHandleLen = data[0];
            std::string &&srcHandle = std::string(data+1, (data+1)+srcHandleLen);
            // step 2 - get the message
            uint8_t numHandles = data[srcHandle.size()+1];
            int start = srcHandle.size()+2;
            int i=start, numHan=0;
            while(i < data_len+start && numHan < numHandles){
                int hanLen = data[i];
                i+=(hanLen+1);
                numHan++;
            }
            // Got to the message (maybe check the )
            std::string &&msg = std::string((char*)(data+i));
            
            std::cout << std::endl << srcHandle << ": " << msg << std::endl;
            return true;
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
    
       MCommandParser parser;
       parser.parse(s);
       uint8_t *pkt = buildMPacket(parser);
       uint16_t pkt_len;
       memcpy(&pkt_len, pkt, 2);
       MultiCastParse(pkt_len - 3, pkt+3);
       

}