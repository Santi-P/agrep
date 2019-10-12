#include <iostream>
#include "agrep.hpp"
#include "levenshtein.hpp"
#include "bit_pattern.hpp"
#include "FSA.hpp"
#include "FSAWrapper.hpp"
#include "recursive_descent.hpp"


typedef FSA<unsigned short int> FSA16;


int main(int argc, char** argv) {
    std::cout<< "HELLO";

    std::string text = "ccccccccfccccc";

// (a|*(bc) causes segfault
// (|a) segfault
// "(a|)" throws

// first char bug

// look into jumping too much to the end
// epsilon jumps are not correct
// modify do concat
RecDesc parser("a(c)*(d|f)c");

for (auto i : parser.tokenized_input){
    std::cout<< i <<std::endl;
    }

    auto automat = parser.parse();
    Agrep a; 
    a.search_fsa(automat, text);


Agrep b; 

b.search_file("jesus",argv[1]);





    // Agrep a; 

    // std::string pattern = "bbb";
    // std::string pattern_2 = "aaa";
    // std::string text = "cccccccccccccbbbbbaaaaaccccccc";
    // a.search_fsa(pattern, pattern_2,text);
  
    // FSA16 fsa_1(pattern);
    // FSA16 fsa_2(pattern_2);
    // FSA16 fsa_31 = fsa_union(fsa_1,fsa_2);
    // FSA16 fsa_3 = fsa_concat(fsa_1,fsa_31);

    

    // std::ofstream dot("fsa.dot");
    // fsa_3.as_dot(dot);



    //BitPattern b(pattern);

    
    // Agrep a; 
    // a.search_wild_card(pattern, text, 2);
    // a.search(pattern, text);

    /*
    if (argc > 3){
        //std::cerr << "wrong number of args"<<std::endl;

        a.search_file(argv[1], argv[2], std::strtoul(argv[3],NULL,10));

        //exit(1);
    }

    else if (argc == 3){
        a.search_file(argv[1], argv[2]);

    }

    else{
        std::cerr<<"wrong number of arguments";
        exit(1);
    }


    */



    //a.search(pattern, text);
}