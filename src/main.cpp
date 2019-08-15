#include <iostream>
#include "agrep.hpp"

int main() {

    std::cout << "AGREP" << std::endl;
    Agrep a;
    auto mask =  a.build_mask(10);
    for(auto i : mask){
        std::cout<< i <<" "<<std::endl; 
    }

}