#include <iostream>
#include <vector>
#include <string> 


class Agrep{
    public: 
    Agrep(){
        // std::cout<< "constructed class" << std::endl;
    }

    // crappy test version
    bool search(std::string pattern, std::string text){
        std::vector<bool> bm = build_mask(pattern.length());
        bm[0] = 1;

        for( int i = 1 ; i < text.length(); ++i){
            std::vector<bool> next_bm = build_mask(pattern.length());
            next_bm[0] = 1;
            char c = text[i];
            std::cout<< i <<std::endl;

        for (int j = 1; j < pattern.length(); ++j){
        
                if (bm[j-1] == 1 and pattern[j]==text[i])
                    {
                        next_bm[j] = true; 
                    }
                else{
                    next_bm[j] = false;
                    }
            

            
        }

        if (bm[pattern.length()-1] == true){
            std::cout<< "FOUND! pattern matches at char " << std::endl;
            std::cout<< i - pattern.length()-1 + 2 <<std::endl;
            return 1;
        }


        bm = next_bm;


        for (auto c : pattern){
            std::cout<< c <<" ";
            }
        std::cout<<std::endl;

        for (auto i : bm){
            std::cout<< i <<" ";
            }





        std::cout<<c;
        std::cout<<std::endl;

        }
        std::cout<<std::endl;

        return 0; 
    }



    std::vector<bool>  build_mask(unsigned size){ 
        std::vector<bool> tmp_mask(size, 0); 
        
        return tmp_mask;
    }

    std::vector<bool> bitmask; 

};