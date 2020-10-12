#ifndef PARSERS_H
#define PARSERS_H
#include "Utils.hpp"

/*=====================CommandValidator===========================*/
class CommandValidator{
    public:
        static const int MAX_INPUT;
        static bool validate(std::string &input, int actualSize);
};

class MCommandValidator{
    private:
        static const std::string FORMAT_MESSAGE;
    public:

        static int findNumHandles(std::string &input);
        static bool validate(std::string &minput);
};
class BCommandValidator{
    private:
        static const std::string FORMAT_MESSAGE;
    public:
        static bool validate(std::string &binput);
};
class LCommandValidator{
   private:
        static const std::string FORMAT_MESSAGE;
    public:
        static bool validate(std::string &Linput);

};
class ECommandValidator{
   private:
        static const std::string FORMAT_MESSAGE;
    public:
        static bool validate(std::string &Einput);

};



class CommandParser{
    protected:
        char cmd;
    public:
        static char getCommand(std::string &input);
        static uint16_t getPacketLen(uint8_t *pkt);
        virtual void parse(std::string &input) = 0;
};

class MCommandParser: public CommandParser{
    private:
        std::queue<std::string> messages;
        std::vector<std::string> destHandles;
        uint8_t numHandles;// only 1-9
    public:

        void parse(std::string &input);
        inline std::queue<std::string> &getMessages(){return this->messages;};
        inline uint8_t &getNumHandles(){return this->numHandles;}
        inline std::vector<std::string> &getDestHandles(){return this->destHandles;}
        inline char &getCommand(){return this->cmd;}
};

class BCommandParser: public CommandParser{
    private:
        std::queue<std::string> messages;
    public:
        void parse(std::string &input);
        std::queue<std::string> &getMessages(){return this->messages;};
        char &getCommand(){return this->cmd;}
};



/*======================== Packet Parsers================================================*/


class PacketParser{
    public:
        virtual void parse(uint8_t *pkt, uint16_t len) = 0;
};
class LoginPacketParser: public PacketParser{
    private:
        std::string handName;
    public:
        // Format: <hdr, 1byte handle lenght then handle no padding/nulls
        void parse(uint8_t *pkt, uint16_t len);
        std::string & getHandName(){return this->handName;}

};
/* Client and Server both use */
class BroadcastPacketParser: public PacketParser{
    private:
        std::string message;
        std::string srcHandle;
    public:
        void parse(uint8_t *pkt, uint16_t len);
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
        void parse(uint8_t *pkt, uint16_t len);
        std::vector<std::string> &getDestHandles(){return this->destHandles;};
        std::string &getMessage(){return this->message;}
        std::string &getSourceHandle(){return this->srcHandle;}
        uint8_t &getNumHandles(){return this->numHandles;}
};

#endif