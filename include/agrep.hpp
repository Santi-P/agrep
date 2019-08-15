#include <iostream>
#include <vector>
#include <string> 


class Agrep{
    public: 
    Agrep(){
        // std::cout<< "constructed class" << std::endl;
    }


    std::vector<bool>  build_mask(unsigned size){ 
        std::vector<bool> tmp_mask(size, 0); 

        return tmp_mask;
    }

    std::vector<bool> bitmask; 

};