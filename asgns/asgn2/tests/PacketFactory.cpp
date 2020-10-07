#include <iosteam>
#include <vector>
#include <numeric>
#include <queue>

// These can be used to parse the individual fields
class Command{

    protected:
        char cmd;
        Command(char cmd)
        : cmd(cmd)
        {
        }
    public:
        virutal bool nextMsg() = 0;

};
class VerbalCommand: public Command{
    protected:
        std::queue<std::string> split_msg; // split by 200 chars
}
class ActionCommand: public{
    protected:
}

class MCommand : public VerbalCommand{
    private:
        uint8_t numHandles;
        std::vector<std::string> dest_handles;
        CommandMParsers parser;
        // getters and setters 
    public:
        MCommand(std::vector<std::string> cmd)
           :parser(cmd)
           {
               // after intializing parser call specific functions here
               this->cmd = std::toupper(cmd[1]);
               this->dest_handles = parser.getDestHandles();
               this->split_msg = parser.getMsg();

           }
        bool nextMsg(){
            if(this->split_msg.size() <= 0)
                return false

            this->split_msg.pop();
            return true;
        }
};

class BCommand : public VerbalCommand{
    public:
        std::string msg;
};
class LCommand : public VerbalCommand{
    public:
        std::string msg;
        bool nextMsg(){return false;}
};


class CommandParsers{
    public:
        static char getCommand(std::vect<std::string> cmd){
           return cmd[0][1];
        }
}
class CommandMParsers{
    private:
        std::vector<std::string> cmd;
        uin8_t flag;
        uint8_t numHands;
    public:

        CommandMParsers(std::vector<std::string> cmd)
            : cmd(cmd)
        {

        }
        void parse(){

        }
}
class CommandFactory{
    private:
        // assume msg is 
       std::vector<std::string> produceMCommand(std::vector<std::string> cmd){
           uin8_t numHandles = std::stoi(cmd);
           std::vector<std::string> dest_handles;
           std::string msg;
           int i;
           for(i = 2; i < numHandles+2 && i< cmd.size(); i++){
                dest_handles.push_back(cmd[i]);
           }
           if(i< cmd.size()-1){
               // there is a message
                msg = cmd[i+1 .. cmd.size()-1]

           }else{
               // no message return '/n'
               msg = '\n';
           }
       }
    public:
        static Command getCommand(std::vector<std::string> cmd){
            switch (std::stoupper(cmd[1]))
            {
            case 'M'
                parseMcommand();
                return MCommand()
                break;
            
            default:
                break;
            }

        }

};

class PacketFactor{
    public:
        uint8_t *buildPacket(Command &cmd, TCPClient &client){
            uint8_t *transBuff = client.transBuff; 
            memset(transBuff, 0, MAX_BUFF);
            if(dynamic_cast<MCommand*>(cmd)){
                // do stuff to build pakcet
                MCommand *mCmd = cmd;

            }
        }

}


/* step 1 - parse the input and get a command back
   step 2 - build Packet (have a function called getMsg(based on type)
 */

class PacketFactory{

    public:
        static uint8_t *getPacket(Command type){
            uint8_t *pkt;
            switch ()
            {
            case /* constant-expression */:
                /* code */
                pkt=type.fomatPacket();
                break;
            
            default:
                break;
            }

        }

};