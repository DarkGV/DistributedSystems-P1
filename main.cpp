#include "Browser.hpp"

int main(){
    Browser b;

    try{
        b.reading();
    }
    catch(char const* e){
        std::cout << e << std::endl;
    }
}