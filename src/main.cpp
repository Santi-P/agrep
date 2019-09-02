#include <iostream>
#include "agrep.hpp"
#include "levenshtein.hpp"

int main() {
    std::string pattern = "aabac";
    std::string text = "aabaacaabacab";


    std::cout << "AGREP" << std::endl;
    unsigned i = levenshtein("hoi", "hoo");
    std::cout<< i <<std::endl;
    Agrep a; 
    a.search(pattern, text);

    a.compile("hello");
}