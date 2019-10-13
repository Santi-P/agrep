// fsa-test.cpp
// TH, UP 2019
// Test der FSA16-Klasse
// Compiler: gcc -std=c++11 / cl

#include <iostream>
#include <fstream>

#include "FSA.hpp"
#include "FSAWrapper.hpp"

typedef FSA<unsigned short int> FSA16; // FSA mit 16-Bit-Zust�nden

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        std::ifstream words_in(argv[1]);
        if (words_in)
        {
            FSA16 lex(words_in,false);
            std::ofstream dot_lex("lex.dot");
            lex.as_dot(dot_lex);
            std::cerr << "lex.dot erzeugt\n";
            std::cerr << "lex_fsa=" << lex << "\n";
            std::ofstream lex_bin("lex_fsa.fsa", std::ofstream::binary);
            lex.serialize(lex_bin);
            lex_bin.close();
            std::cerr << "lex_fsa.fsa erzeugt\n";
        }
        return 0;
    }

    // Hinweis: dot erlaubt auch mehrere Graphen in einer Datei
    // und erzeugt dann auch mehrere PDFs
    std::ofstream dot("fsa.dot");

    FSA16 fsa1("Taschen");
    FSA16 fsa2("Lehr");
    FSA16 fsa3("Koch");
    FSA16 fsa4("Buch");
    // fsa1.as_dot(dot);

    // FSA16 hiha = fsa_concat(FSA16("h"),fsa_union(FSA16("i"),FSA16("a")));
    // FSA16 hiha_plus = fsa_concat(hiha,fsa_closure(hiha));
    // hiha_plus.as_dot(dot);

    // FSA16 fsa5 =  FSA16 fsa5b = fsa_closure(fsa5);
    FSA16 fsa5 = fsa_closure(fsa_union(fsa_union(fsa1, fsa2), fsa3));
    fsa5 = fsa_concat(fsa5,fsa4);
    fsa5.as_dot(dot);
    // FSA16 fsa5b = fsa_closure(fsa5);
    //fsa5b.as_dot(dot);

    // std::ofstream bin1("fsa5.fsa",std::ios_base::binary);
    // fsa5.serialize(bin1);
    // bin1.close();

    // FSA16 fsa6 = fsa_concat(fsa5,fsa4);
    // std::cout << fsa6 << "\n";
    // fsa6.as_dot(dot);

    // // Globale Operatoren
    // FSA16 fsa7 = (fsa1+fsa2+fsa3) * fsa4;
    // std::cout << fsa7 << "\n";
    // fsa7.as_dot(dot);

    // // Verkettung eines FSA16 mit einem String ("asymmetrische" Operatoren)
    // // Frage: warum geht das?
    // FSA16 fsa8 = fsa7 * "Verlag";
    // std::cout << "fsa8=" << fsa8 << "\n";
    // fsa8.as_dot(dot);

    // // ur* gro� (tante|vater|mutter)
    // FSA16 fsa9 = fsa_closure(FSA16("ur")) * FSA16("gro�") * (FSA16("tante") + FSA16("vater") + FSA16("mutter"));
    // fsa9.as_dot(dot);

    // // Test der Bin�rdateien
    // // 1. Schreiben
    // std::ofstream bin2("fsa9.fsa", std::ofstream::binary);
    // std::cout << "fsa9=" << fsa9 << "\n";
    // fsa9.serialize(bin2);
    // bin2.close();

    // // 2. Wiedereinlesen
    // std::ifstream bin_in("fsa9.fsa", std::ifstream::binary);
    // FSA16 fsa10(bin_in,true);
    // std::cout << "fsa10=" << fsa10 << "\n";
    // fsa10.as_dot(dot);

    // // Test der Iteratoren: tabellarische Ausgabe des Automaten
    // // Iteration �ber die Zust�nde
    // for (auto q = fsa10.states_begin(); q != fsa10.states_end(); ++q) {
    //   // Iteration �ber die �berg�nge, die von q ausgehen
    //   for (auto t = fsa10.transitions_leaving(q); t != fsa10.end_transitions(); ++t) {
    //     std::cout << q << "\t";
    //     if (t->letter == FSA16::EPSILON)
    //       std::cout << "EPSILON";
    //     else
    //       std::cout << t->letter;
    //     std::cout << "\t" << t->target_state << "\n";
    //   }
    // }

    // dot.close();

    // // Hashing
    // std::cout << std::endl;
    // std::cout << "hash(fsa1) =  " << fsa1.hash() << "\n";
    // std::cout << "hash(fsa2) =  " << fsa2.hash() << "\n";
    // std::cout << "hash(fsa3) =  " << fsa3.hash() << "\n";
    // std::cout << "hash(fsa4) =  " << fsa4.hash() << "\n";
    // std::cout << "hash(fsa5) =  " << fsa5.hash() << "\n";
    // std::cout << "hash(fsa6) =  " << fsa6.hash() << "\n";
    // std::cout << "hash(fsa7) =  " << fsa7.hash() << "\n";
    // std::cout << "hash(fsa8) =  " << fsa8.hash() << "\n";
    // std::cout << "hash(fsa9) =  " << fsa9.hash() << "\n";
    // std::cout << "hash(fsa10) = " << fsa10.hash() << "\n";

}
