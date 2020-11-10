#include <iostream>

class base{
    public:
        int i;
        int getI(){
            return i;
        }
};

class baseA: public base{
    public:
        baseA(int n){
            this->i = n;
        }
};

void fun(base *b){
    std::cout << b->getI() << std::endl;
}
void f(int *a){

    std::cout << *a << std::endl;
}

int main(){
    baseA a(1);
    //fun((base *)&a);
    f(&a.getI());

    return 0;
}