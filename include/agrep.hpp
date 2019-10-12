#include <iostream>
#include <vector>
#include <string> 
#include "boost/dynamic_bitset.hpp"
#include <limits> 
#include <fstream>
#include <bitset>
#include "FSA.hpp"
#include "FSAWrapper.hpp"

typedef FSA<unsigned short int> FSA16;
typedef std::bitset<32> BitSet;
const size_t shortmax = std::numeric_limits<unsigned short>::max();

class Agrep{
    public: 
    Agrep(){

        compiled_masks.resize(256);
        transition_masks_1.resize(shortmax);
        transition_masks_2.resize(shortmax);
        epsilons.reserve(256);
    }


    void compile(std::string pattern){
        auto len = pattern.length();
        // tmp fix
        std::bitset<32> b;
        //boost::dynamic_bitset<> b(len);

        size_t hi = std::numeric_limits<unsigned char>::max();

        std::vector<std::bitset<32>> masks ;
        masks.reserve(hi);

        for (int i = 0; i < hi; ++i){
            masks.push_back(b);
        }


        for (unsigned j = 0; j < len; ++j){ 
            unsigned char c = pattern[j];

            if (masks[c].any()) continue; 

            //boost::dynamic_bitset<> tmp_bs(len);
            std::bitset<32> tmp_bs;

            for(unsigned k = 0; k < len; ++k){
                if (c == pattern[k]){
                    tmp_bs.set(k);
                }
            }
            masks[c] = tmp_bs;
            }
        compiled_masks = masks;
    }


    bool search_file(std::string pattern, std::string f_name , unsigned errors = 0){
        bool exact = (errors == 0); 
        std::cout<< "is exact"<<exact; 

        compile(pattern);
        size_t m = pattern.length();


        std::ifstream file(f_name);
        std::string text;
        int line_num = 1;

        // boost::dynamic_bitset<> b(m);

        unsigned Rs_size = errors + 1;
        std::vector<std::bitset<32>> Rs; 
        std::vector<std::bitset<32>> prevs; 


        Rs.resize(Rs_size);
        std::bitset<32> s_i;



        for(int i=1; i < Rs_size; ++ i){
            Rs[i] = Rs[i-1];
            Rs[i] <<= 1; 
            Rs[i].set(0);
        }// print out bit matrix


        while (std::getline(file, text)) {
        

        line_num++;
        if (text.length() <1) continue; 
        for(int i = 0; i < text.length() ; ++i){

                unsigned char c = text[i];
                s_i = compiled_masks[c];
                prevs = Rs;
                //temp_prev = Rs[0];

                Rs[0] <<= 1; 
                Rs[0].set(0);

                Rs[0] &= s_i;
                //std::cout<<Rs[0]<<std::endl;


                for (int j = 1; j < Rs_size; j++){
                    Rs[j] <<= 1; 
                    Rs[j].set(0);
                    Rs[j] &= s_i;

                    Rs[j]|=((Rs[j-1]|prevs[j-1]) <<= 1)|prevs[j-1]; 
                    

                }

                if (Rs[Rs_size - 1].test(m-1)) {
                    std::cout<<"found real match at:"<< i - m + 2 <<std::endl;
                    std::cout<< text<<std::endl;
                    }


                // else {
                //     for (int j = 1; j < Rs_size; j++){

                //     if(Rs[j].test(m + j -1)){ std::cout<<"found partial match at:"<< i - m + 2 <<std::endl;
                //         std::cout<< text<<std::endl;}
                //     }
                // }


            //s_i = compiled_masks[c];
            }     
        }

        return 0;
        }



        bool search_wild_card(std::string pattern, std::string text ,unsigned wild_split, unsigned errors =0){

            bool exact = (errors == 0); 

            compile(pattern);
            size_t m = pattern.length();
            unsigned Rs_size = errors + 1;
            std::vector<std::bitset<32>> Rs; 
            std::vector<std::bitset<32>> prevs; 

            Rs.resize(Rs_size);
            std::bitset<32> s_i;
            std::bitset<32> temp_prev;
            std::bitset<32> s_sharp;

            for(int i=1; i < Rs_size; ++ i){
                Rs[i] = Rs[i-1];
                Rs[i] <<= 1; 
                Rs[i].set(0);
            }// print out bit matrix



            for(unsigned i = 0; i < wild_split; ++i){
                s_sharp.set(i);
            }
            std::cout<<s_sharp;

            

            for(int i = 0; i < text.length() ; ++i){

                unsigned char c = text[i];
                s_i = compiled_masks[c];
                prevs = Rs;
                //temp_prev = Rs[0];

                Rs[0] <<= 1; 
                Rs[0].set(0);

                Rs[0] &= s_i;
                //Rs[0] = Rs[0] | (Rs[0] & s_sharp);



                for (int j = 1; j < Rs_size; j++){
                    Rs[j] <<= 1; 
                    Rs[j].set(0);
                    Rs[j] &=( s_i|((Rs[j-1]|prevs[j-1])<<1)|prevs[j-1]); 


                }



                for (int j = 0; j < Rs_size; j++){
                    std::cout<<"old "<<Rs[j]<<std::endl;
                    Rs[j] |= ((prevs[j-1] & s_sharp));
                    std::cout<< "new " << Rs[j] <<std::endl;
                }
                 if (Rs[Rs_size - 1].test(m-1)) std::cout<<"found real match at:"<< i - m + 2 <<std::endl;
            }     


        return true;

        }




        bool search(std::string pattern, std::string text, unsigned errors = 0){


            bool exact = (errors == 0); 

            compile(pattern);
            size_t m = pattern.length();
            unsigned Rs_size = errors + 1;
            std::vector<std::bitset<32>> Rs; 
            std::vector<std::bitset<32>> prevs; 
        

            Rs.resize(Rs_size);
            std::bitset<32> s_i;
            std::bitset<32> temp_prev;



            for(int i=1; i < Rs_size; ++ i){
                Rs[i] = Rs[i-1];
                Rs[i] <<= 1; 
                Rs[i].set(0);
            }// print out bit matrix



            for(int i = 0; i < text.length() ; ++i){

                unsigned char c = text[i];
                s_i = compiled_masks[c];
                prevs = Rs;
                //temp_prev = Rs[0];

                Rs[0] <<= 1; 
                Rs[0].set(0);

                Rs[0] &= s_i;
                //std::cout<<Rs[0]<<std::endl;


                for (int j = 1; j < Rs_size; j++){
                    Rs[j] <<= 1; 
                    Rs[j].set(0);
                    Rs[j] &=( s_i|((Rs[j-1]|prevs[j-1])<<1)|prevs[j-1]); 

                }

                if (Rs[Rs_size - 1].test(m-1)) std::cout<<"found real match at:"<< i - m + 2 <<std::endl;
            }     

        return true;

        }



    // 1 * (2|3)
    bool search_fsa(FSA16 fsa_3,std::string text, unsigned errors = 0){


        auto m = fsa_3.states.size(); 
        if (m > 32){
            std::cout<< "overflowing";
            exit(1);
        }

//     FSA16 fsa_1(pattern_1);
//     FSA16 fsa_2(pattern_2);
//     FSA16 fsa_31 = fsa_union(fsa_1,fsa_2);
//     FSA16 fsa_3 = fsa_concat(fsa_1,fsa_31);


    



    //std::cout<< fsa_1.states.size(); 
    int final = 0; 
    std::cout<< "start: " << fsa_3.start_state() <<std::endl;
    for (int i = 0; i < fsa_3.states.size();++i){
        //std::cout<< "state" << i << " " <<fsa_3.states[i].is_final <<std::endl;
        if (fsa_3.states[i].is_final) final = i; 
        std::cout<<"state: "<< i <<std::endl;

        for (auto j = fsa_3.transitions_leaving(i); j != fsa_3.end_transitions(); ++j )
            {        
            if (j->letter == 0) {
                std::cout<<"EPSi"<<" " << j->target_state << std::endl;
                std::bitset<32> b;
                //b.set(i);
                b.set(j->target_state);
                
                transition_masks_1[i] |= b;
                std::cout<< "adding: "<< i<<std::endl;  
                epsilons.push_back(i);
                std::cout<< "adding " << i << "to epsilon"<< std::endl;
                //std::cout<<b; 

                // for(int q = 0; q < i; ++q){
                //     b.set(q);
                // }

                //b.set(i);

            //     //std::cout<<b <<std::endl;
            //     std::bitset<32> tmp_copy_b = b;


                
            //     //std::cout<< b<<std::endl; 
            //     //std::cout<< tmp_copy_b <<std::endl;
            // //     for(int i=0 ;i < 32; i+=16){
            // //         //std::cout<<tmp_copy_b<<std::endl;
            // //         unsigned short k =  static_cast<unsigned short>(tmp_copy_b.to_ulong());
            // //         transition_masks_1[k] = b; 
            // //         tmp_copy_b >>= 16; 
            // //     }

            // // }
            //     std::bitset<32> post_jump = b;
            //     post_jump.set(j->target_state);




            //         //std::cout<<tmp_copy_b<<std::endl;
            //     unsigned short k =  static_cast<unsigned short>(tmp_copy_b.to_ulong());
            //     transition_masks_1[k] = post_jump; 
                
            //     tmp_copy_b >>= 16; 

            //     unsigned short l =  static_cast<unsigned short>(tmp_copy_b.to_ulong());
            //     transition_masks_2[l] =  post_jump; 

            }




            else {
                
                
                std::cout<<j->letter <<"->" << j->target_state << std::endl;
                
                // add here to get classes

                char c = j->letter;
                std::bitset<32> b;
                b.set(j->target_state);
                b.set(i);
                //std::cout<<b <<std::endl;
            

                compiled_masks[c] |= b;


                }
            }   

        }


    for (auto j : epsilons){
        std::cout<<j<<std::endl;
    }

    std::cout<< m <<std::endl;



    unsigned Rs_size = errors + 1;
    std::vector<std::bitset<32>> Rs; 
    std::vector<std::bitset<32>> prevs; 


    Rs.resize(Rs_size);
    std::bitset<32> s_i;
    std::bitset<32> temp_prev;


    
    // initialize shit matrix
    for(int i=1; i < Rs_size; ++ i){
               // Rs[i-1].set(fsa_3.start_state());
                Rs[i] = Rs[i-1];
                Rs[i] <<= 1; 
                Rs[i].set(0);
            }// print out bit matrix


    Rs[0].set(fsa_3.start_state());
    // here is the roblem 
    std::cout<<" reached me  " <<std::endl;

    apply_epsilon(Rs[0]);

                Rs[0] <<= 1; 
                Rs[0].set(0);


    std::cout<<Rs[Rs_size - 1]<<" start " <<std::endl;


    for(int i = 0; i < text.length() ; ++i){
                Rs[0].set(fsa_3.start_state());

                apply_epsilon(Rs[0]);


                unsigned char c = text[i];
                s_i = compiled_masks[c];

                std::cout<<Rs[Rs_size - 1]<<" at text: " <<text[i]<<std::endl;

                std::cout<< s_i <<" mask"<<std::endl;
                prevs = Rs;
                //temp_prev = Rs[0];

                Rs[0] <<= 1; 
                Rs[0].set(0);
                std::cout<<Rs[Rs_size - 1]<<" after shift " <<std::endl;


                Rs[0] &= s_i;
                //std::cout<<s_i<<std::endl;
                std::cout<<Rs[Rs_size - 1]<<" after ands " <<std::endl;

                //
                apply_epsilon(Rs[0]);

                std::cout<<Rs[Rs_size - 1]<<" after jumps  " <<std::endl<<std::endl;



                // std::bitset<32> tmp_copy = Rs[0];
                // unsigned short k =  static_cast<unsigned short>(tmp_copy.to_ulong());
                // Rs[0] |= transition_masks_1[k]; 

                
                // tmp_copy >>= 16; 

                // unsigned short l =  static_cast<unsigned short>(tmp_copy.to_ulong());
                // std::cout<< transition_masks_2[l]<<std::endl;

                // Rs[0] |= transition_masks_2[l]; 

                // for (int j = 1; j < Rs_size; j++){
                //     std::cout<<"bebop";
                //     Rs[j] <<= 1; 
                //     Rs[j].set(0);
                //     Rs[j] &=( s_i|((Rs[j-1]|prevs[j-1])<<1)|prevs[j-1]); 


                //     std::bitset<32> tmp_copy = Rs[j];
                //     unsigned short k =  static_cast<unsigned short>(tmp_copy.to_ulong());
                    
                //     Rs[j] |= transition_masks_1[k]; 
                    
                //     tmp_copy >>= 16; 

                //     unsigned short l =  static_cast<unsigned short>(tmp_copy.to_ulong());
                //     Rs[j] |= transition_masks_2[l]; 


                // }

                //std::cout<<Rs[Rs_size - 1]<<std::endl;

                std::cout<<"final "<<final<<std::endl;
                if (Rs[Rs_size - 1].test(final)) {
                    
                    std::cout<<"found real match at:"<< i  <<std::endl;
                    Rs[Rs_size - 1].reset();
                    
                    }
            }     





    // for(int i = 0; i < shortmax; ++i){
    //     auto m = transition_masks_1[i];

    //     auto n = transition_masks_2[i];

    //     if (m.any()){
    //         std::cout<< i <<std::endl;
    //         std::cout<<m<<std::endl;
    //     }

    //     if (n.any()){
    //         std::cout<< i <<std::endl;
    //         std::cout<<n<<std::endl;
    //     }

    // }

    // for(auto i :epsilons){
    //     std::cout<<i<<std::endl;
    //     std::cout<<transition_masks_1[i]<<std::endl;

    // }
    return true; 

    }

    // why the fuck
    BitSet apply_epsilon(std::bitset<32> bs){
        //std::cout<< "num eps " << epsilons.size();  
        for(int k = 0; k < epsilons.size(); ++k){
            unsigned i = epsilons[k];
            //std::cout<<i; 
            if(bs.test(i)){

                bs |= transition_masks_1[i];
            }

        }
        return bs;
    }

    void compile_range(){}

    int to_int(BitSet bs){

        int res = int(bs.to_ulong());
        return res;
    }

    std::vector<std::bitset<32>> compiled_masks;
    std::vector<std::bitset<32>> transition_masks_1;


    std::vector<std::bitset<32>> transition_masks_2;

    std::vector<int> epsilons; 




};
