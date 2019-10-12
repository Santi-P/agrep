#include <iostream> 
#include <string> 
#include <vector> 
#include <deque>
#include <string> 
#include <vector> 
#include <map> 
#include "FSA.hpp"
#include "FSAWrapper.hpp"
#include <cassert>


// todo sanitize


typedef FSA<unsigned short int> FSA16;

class RecDesc{
    public:
    RecDesc(){
        std::cout<< "hello"<<std::endl;
    }

    RecDesc(std::string in_string){
        to_parse = in_string;
        tokenized_input = shitty_tokenize(in_string);
        len_inp = tokenized_input.size();
    }

    FSA16 parse(){
        FSA16 res = regex();
        std::ofstream dot("fsa.dot");
        res.as_dot(dot);
        return res; 
    }


    void start(){     

    }
    

    std::string to_parse; 
    std::vector<std::string> tokenized_input;
    unsigned len_inp;
    unsigned current_tok = 0; 
    bool valid = true; 


    bool is_end(){
        return current_tok > len_inp - 1;
    }



    std::vector<std::string> tokenize(std::string s){
    unsigned last = 0; 
    std::vector<std::string> res; 
    res.reserve(s.length());

    for(int i=0; i<s.length() ; ++i){

        char curr = s[i];

        if (curr == '|' || curr =='*' || curr =='(' ||  curr == ')'){
            std::string tmp_string = "";
            tmp_string += curr; 
            res.push_back(s.substr(last, i-last));
            res.push_back(tmp_string);
            last = i + 1;
        }

    }

    if (last != s.length()){
        res.push_back(s.substr(last+1,s.length() ));
        }
    return res; 
    }

    std::vector<std::string> shitty_tokenize(std::string s){
        std::vector<std::string> res; 
        for(int i = 0; i < s.length(); i++){
            std::string tmp;
            tmp += s[i];

            res.push_back(tmp);
        }
        return res; 
    }

    // helper functions

    std::string peek(){
    if (is_end()){
        return "";
        }
        return tokenized_input[current_tok];
    }

    std::string next(){
    //assert(current_tok < len_inp);
    std::string res = tokenized_input[current_tok ];
    current_tok += 1; 
    return res;
    }

    bool eat(std::string s){
    if (s == tokenized_input[current_tok]) { 
        current_tok += 1; 
        return true;
        } 

    else{
        throw "invalid syntax";

        }

    return false;
    }



    // grammar functions
    FSA16 regex(){

        FSA16 res =  term();
        if ( (not is_end() )and peek() == "|"){
            eat("|");
            FSA16 rhs = regex();
            return fsa_union(res, rhs);
        }

        return res; 
    }

    FSA16 term(){
        // group
        FSA16 res = factor();
        while( not is_end() and peek() != ")" and peek() != "|"){
            FSA16 next_fac = factor();
            res = fsa_concat(res,next_fac);
        }
        return res;
    }

    FSA16 factor(){
        // check for closure
        FSA16 res = base();
       if (peek() =="*"){
            res = fsa_closure(res);

            eat("*");
        }
        return res ;

    }

    FSA16 base(){
        if (peek() == "("){
            eat("(");
            FSA16 res = regex();
            eat(")");
            std::cout<<"post paren "<< tokenized_input[current_tok] <<std::endl;
            return res; 
        }

        FSA16 fsa(next());
        std::cout<<"nexting"<<std::endl;
        return fsa;
        }
    };

