#include <iosteam>
#include <vector>

// These can be used to parse the individual fields
class Command{

    protected:
        char cmd;
        Command(char cmd)
        : cmd(cmd)
        {
        }
};
class MCommand : public Command{
    private:
        uint8_t numHandles;
        std::vector<char*> dest_handles;
        char msg[200];
        // getters and setters 
    public:
        MCommand(uint8_t numHands, std::)

};
class BCommand : public Command{
    public:
        char msg[200];
};

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