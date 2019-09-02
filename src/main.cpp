#include <iostream>
#include "agrep.hpp"
#include "levenshtein.hpp"

int main() {
    std::string pattern = "nanaa";
    std::string text = "aabaacaabacabadsfasdfasdfas;dflkj bananaasdfsdf fuck";


    std::cout << "AGREP" << std::endl;
    unsigned i = levenshtein("hoi", "hoo");
    std::cout<< i <<std::endl;
    Agrep a; 
    a.search(pattern, text);


}