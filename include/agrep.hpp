#include <iostream>
#include <vector>
#include <string> 
#include "boost/dynamic_bitset.hpp"
#include <limits> 
#include <fstream>

class Agrep{
    public: 
    Agrep(){
        // std::cout<< "constructed class" << std::endl;
    }


    void compile(std::string pattern){
        auto len = pattern.length();
        boost::dynamic_bitset<> b(len);
        size_t hi = std::numeric_limits<unsigned char>::max();

        std::vector<boost::dynamic_bitset<>> masks(hi, b ) ;


        for (unsigned j = 0; j < len; ++j){ 
            unsigned char c = pattern[j];

            if (masks[c].any()) continue; 

            boost::dynamic_bitset<> tmp_bs(len);

            for(unsigned k = 0; k < len; ++k){
                if (c == pattern[k]){
                    tmp_bs.set(k);
                }
            }
            masks[c] = tmp_bs;

            }
        compiled_masks = masks;
    }


    bool search_file(std::string pattern, std::string f_name){
        compile(pattern);
        size_t m = pattern.length();


        std::ifstream file(f_name);
        std::string text;
        int line_num = 1;

        while (std::getline(file, text)) {
        

        line_num++;
        if (text.length() <1) continue; 
        boost::dynamic_bitset<> b(m);
        b.set(0);


        for(int i = 0; i < text.length() ; ++i){

            b<<=1; 
            b.set(0);
            unsigned char c = text[i];
            boost::dynamic_bitset<> s_i = compiled_masks[c];
            b&=s_i; 

            if( b.test(m-1)){
                std::cout<<"found at :"<< line_num << ":" << i - m + 2<<std::endl;
                std::cout<<text <<std::endl;
                }
            }     
        }

        return 0;
    }


    bool search(std::string pattern, std::string text){
        compile(pattern);
        // fix me 
        size_t m = pattern.length();
        boost::dynamic_bitset<> b(m);
        
        b.set(0);

        for(int i = 0; i < text.length() ; ++i){
            b<<=1; 
            b.set(0);
            unsigned char c = text[i];
            boost::dynamic_bitset<> s_i = compiled_masks[c];
            b&=s_i; 

            //std::cout<< b<<std::endl;

            if( b.test(m-1)){
                std::cout<<"found at :"<< i - m + 2<<std::endl;
            }
        }     
    }

    // crappy test version
    // bool search(std::string pattern, std::string text){
    //     const int m = pattern.length();
    //     std::bitset<5> bm ;
        
    //     bm.set(0);


    //     for( int i = 1 ; i < text.length(); ++i){
    //         std::bitset<5> next_bm;
    //         char c = text[i];
    //         if (pattern[0] == c){
    //             next_bm.set(0);
    //         }

    //         std::cout<< i <<std::endl;


    //     for (int j = 1; j < m; ++j){
                
    //             if (bm[j-1] == 1 and pattern[j]==text[i])
    //                 {
    //                     next_bm[j] = true; 
    //                 }
    //             else{
    //                 next_bm[j] = false;
    //                 }
            

            
    //     }

    //     if (bm[m-1] == true){
    //         std::cout<< "FOUND! pattern matches at char " << std::endl;
    //         std::cout<< i - m -1 + 2 <<std::endl;
    //         return 1;
    //     }


    //     bm = next_bm;

   
    //     for (auto c : pattern){
    //         std::cout<< c <<" ";
    //         }
    //     std::cout<<std::endl;

    //     for (int i = 0 ; i<m ; i++){
    //         std::cout<< bm[i] <<" ";
    //         }





    //     //std::cout<<c;
    //     std::cout<<std::endl;

    //     }
    //     std::cout<<std::endl;

    //     return 0; 
    // }

    std::vector<boost::dynamic_bitset<>> compiled_masks;




};