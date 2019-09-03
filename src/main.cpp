#include <iostream>
#include "agrep.hpp"
#include "levenshtein.hpp"

int main(int argc, char** argv) {

    // std::string pattern = "aabac";
    // std::string text = "aabaacaabacab";


    // std::cout << "AGREP" << std::endl;
    // unsigned i = levenshtein("hoi", "hoo");
    // std::cout<< i <<std::endl;
    if (argc != 3){
        std::cerr << "wrong number of args"<<std::endl;
        exit(1);
    }

    Agrep a; 
    a.search_file(argv[1], argv[2]);

}