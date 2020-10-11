#ifndef PARSERS_H
#define PARSERS_H
#include "Utils.hpp"

/*=====================CommandValidator===========================*/
class CommandValidator{
    private:
        static const int MAX_INPUT;
        static const std::string COMMAND_FORMATS;
    public:
        static bool validate(std::string &input);
};

class CommandParser{
    protected:
        char cmd;
    public:
        static char getCommand(std::string &input);
        virtual void parse(std::string &input) = 0;
};

class MCommandParser: public CommandParser{
    private:
        std::queue<std::string> messages;
        std::vector<std::string> destHandles;
        uint8_t numHandles;// only 1-9
    public:
        void parse(std::string &input)throw (const char *);
        inline std::queue<std::string> &getMessages(){return this->messages;};
        inline uint8_t &getNumHandles(){return this->numHandles;}
        inline std::vector<std::string> &getDestHandles(){return this->destHandles;}
        inline char &getCommand(){return this->cmd;}
};

class BCommandParser: public CommandParser{
    private:
        std::queue<std::string> messages;
    public:
        void parse(std::string &input)throw (const char *);
        std::queue<std::string> &getMessages(){return this->messages;};
        char &getCommand(){return this->cmd;}
};


/*
class LCommandParser: public CommandParser{
    private:
        
    public:


};

*/

/*======================== Packet Parsers================================================*/


class PacketParser{
    protected:
        uint16_t getPacketLen(uint8_t *pkt){
            uint16_t len;
            memcpy(&len, pkt, 2);
            return len;
        }
    public:
        virtual void parse(uint8_t *pkt) = 0;
};
class LoginPacketParser: public PacketParser{
    private:
        std::string handName;
    public:
        // Format: <hdr, 1byte handle lenght then handle no padding/nulls
        void parse(uint8_t *pkt);
        std::string & getHandName(){return this->handName;}

};
/* Client and Server both use */
class BroadcastPacketParser: public PacketParser{
    private:
        std::string message;
        std::string srcHandle;
    public:
        void parse(uint8_t *pkt);
        std::string &getMessage(){return this->message;}
        std::string &getSourceHandle(){return this->srcHandle;}

};

class MulticastPacketParser: public PacketParser{
    private:
        std::vector<std::string> destHandles;
        std::string message;
        std::string srcHandle;
        uint8_t numHandles;// only 1-9
        
    public:
    /* Expand to class for server and client to shar */
        void parse(uint8_t *pkt);
        std::vector<std::string> &getDestHandles(){return this->destHandles;};
        std::string &getMessage(){return this->message;}
        std::string &getSourceHandle(){return this->srcHandle;}
        uint8_t &getNumHandles(){return this->numHandles;}
};

#endif