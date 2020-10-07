#include <iostream>



// These can be used to parse the individual fields
class Command{

    protected:
        char cmd;
        Command(char cmd)
        : cmd(cmd)
        {
        }
    public:
        Command();

};
class Mcommand{
    public:
      uint8_t numHandles;
      bool nextMsg(){
          return true;
      }
};
class BCommand{
    int number;
    bool nextMsg(){
        return false;
    }
};
Command getCommand(char e){
    Command c;
    if(e=='M'){
        c = &Mcommand();
    }else if(e=='B'){
        c=BCommand();
    }

}

int main(){
    Command
    return 0;
}