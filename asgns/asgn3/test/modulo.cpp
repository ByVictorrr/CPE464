
#include <iostream>


int main(){
    int size = 5;
    int counter = 0;
    for (size_t i = 0; i < 20; i++)
    {

        counter++;
        if(counter == 1){
            std::cout << "[ ";
        }

        std::cout << i << ", ";
        /* code */
        if(counter >= 5){
            counter = 0;
            std::cout << " ] " << std::endl;
        }
        
    }
    
}