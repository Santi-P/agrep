// main Agrep class
// SP 13.10.19
// contains member functions for preprocessing (aka building bit masks)
// searching using standard shift-and/or
// preprocessing regular expression and using shift-or to search

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "boost/dynamic_bitset.hpp"
#include <limits>
#include <fstream>
#include <bitset>
#include "FSA.hpp"
#include "FSAWrapper.hpp"
#include <cassert>
#include "recursive_descent.hpp"


const size_t WORD_SIZE = 64;
const size_t shortmax = std::numeric_limits<unsigned short>::max();
const size_t MAX_CHAR= std::numeric_limits<unsigned char>::max();
const size_t MIN_CHAR= std::numeric_limits<unsigned char>::min();

typedef FSA<unsigned short int> FSA16;
typedef std::bitset<WORD_SIZE> BitSet;


//
/// \brief Agrep class for approximate string searching.
/// contains member functions for preprocessing (aka building bit masks)
/// searching using standard shift-and/or
/// preprocessing regular expression and using shift-or to search
class Agrep
{

public:

    /// \brief Default Constructor Initializes some Variables
    Agrep()
    {

        masks.resize(MAX_CHAR);
        transition_masks.resize(MAX_CHAR);
        epsilons.reserve(WORD_SIZE);
    }

    /// \brief preprocess pattern
    /// default is normal pattern.
    /// Masks are generated for the pattern.
    /// If regex is set to true, the pattern would be parsed and
    /// translated into a thompson nfa.
    /// this is then translated again into a set of bitmasks.
    /// \param pattern pattern to be searched
    /// \errors number of allowed errors. default is 0 in other words exact search
    /// \ regex interpret the string as regex or not. default is false
    Agrep(std::string pattern, unsigned errors = 0,bool regex = false)
    {
        Agrep();
        if (regex) compile_regex(pattern,errors);
        else compile_standard(pattern, errors);
    }

    /// \brief preprocess pattern
    /// searchers directly in the file.
    /// \param pattern pattern to be searched
    /// \param f_name file name to search in
    /// \errors number of allowed errors. default is 0 in other words exact search
    /// \ regex interpret the string as regex or not. default is false
    Agrep(std::string pattern, std::string f_name, unsigned errors = 0, bool regex = false)
    {
        Agrep();
        search_file(pattern, f_name, errors, regex);

    }

    // copy and assign operators are not needed in this case
    // all member variables have their own copy/assign constructors


    /// \brief search wrapper for file searching

    /// \param pattern pattern to be searched
    /// \param f_name file name to search in
    /// \errors number of allowed errors. default is 0 in other words exact search
    /// \ regex interpret the string as regex or not. default is false
    bool search_file(std::string pattern, std::string f_name, unsigned errors = 0, bool regex = true)
    {
        pattern_length = pattern.length();

        if (errors > pattern_length)
        {
            std::cerr<< "WARNING: Too many allowed errors"<<std::endl;
        }

        bool found = false;
        if(regex)
        {
            compile_regex(pattern,errors);
        }

        else
        {
            compile_standard(pattern, errors);
        }

        std::ifstream file(f_name);
        std::string text;
        int line_num = 0;


        while (std::getline(file, text))
        {

            line_num++;
            if (text.length() <1) continue;

            else
            {
                int res;
                if (regex)
                {
                    res = search_fsa(text);
                } // end if
                else
                {
                    res = search(text);
                }// end else


                if(res > -1)
                {
                    found = true;
                    // dirty fix

                    if (regex)
                    {
                        std::cout<< f_name<<": "<< line_num<<":"<<res<<std::endl;
                        std::cout<< text <<std::endl;
                        std::cout << "\033[38;5;205m"<<make_empty(text.length(), res)<< "\033[m" <<std::endl;
                    } // end if
                    else
                    {
                        std::cout<< line_num<<":";
                        std::cout<< text.substr(0, res) << "\033[38;5;205m"<<text.substr(res,pattern_length) << "\033[m"
                                 << text.substr(res+pattern_length, text.length()) <<std::endl;

                    }// end else
                }// end if
            }// end else
        }// end while

        return found;

    }// end




private:
    /// \brief preprocessing function for normal patterns.
    /// \param pattern is a string which is to be searched
    /// \param errors is the number of allowed errors (in Levenshtein distance)
    void compile_standard(std::string pattern, unsigned errors = 0)
    {


        if (pattern_length > WORD_SIZE)
        {
            std::cerr<< "Error: Overflowing. Agrep only supports short patterns (max. 64). Consider using grep "<<std::endl;
            exit(1);
        }

        auto len = pattern.length() + 1;
        Rs_size = errors + 1;
        Rs.resize(Rs_size);
        Rs[0].set(); // flip all bits to 1

        // RS[0 ] is guarateed to exist
        // go through all error bit arrays and flip/shift them
        // initialization step
        for(int i=1; i < Rs_size; ++ i)
        {
            Rs[i] = Rs[i-1];
            Rs[i].set();
            Rs[i] <<= 1;
        }// endfor

        masks.resize(MAX_CHAR);

        // initialize masks
        for (int i = 0; i < MAX_CHAR; ++i)
        {
            masks[i].set();
        }// endfor


        for (unsigned j = 0; j < len; ++j)
        {
            unsigned char c = pattern[j];
            if(c =='.')
            {
                wild_card_mask(j);
            }
            // make tmp bit set
            else
            {
                BitSet tmp_bs;
                // initialize to 1s
                tmp_bs.set();

                for(unsigned k = 0; k < len; ++k)
                {
                    if (c == pattern[k])
                    {

                        // flip according to character in pattern
                        tmp_bs.flip(k);
                    }// end if
                }// end for k
                // add to masks
                assert(c < MAX_CHAR);
                masks[c] &= tmp_bs;
            }
        }// end for j

    }// end compile_standard


    /// preprocessing function for regex
    void compile_regex(std::string pattern, unsigned errors = 0)
    {

        RecDesc parser(pattern);
        auto fsa = parser.parse();

        size_t pattern_length = fsa.states.size() + 1;
        if (pattern_length > WORD_SIZE)
        {
            std::cerr<< "Error: Overflowing. Agrep only supports short patterns (max. 64). Consider using grep "<<std::endl;
            exit(1);
        }

        start = fsa.start_state();
        for (int i = 0; i < fsa.states.size(); ++i)
        {

            if (fsa.states[i].is_final) final = i;

            for (auto j = fsa.transitions_leaving(i); j != fsa.end_transitions(); ++j )
            {
                if (j->letter == 0)
                {
                    BitSet b;
                    b.set(j->target_state);
                    b.set(i);
                    transition_masks[i] |= b;
                    // remeber epsilons
                    // maybe change to tree like map
                    epsilons.push_back(i);
                }

                else
                {
                    // add here to get classes

                    char c = j->letter;

                    if(c =='.')
                    {
                        wild_card_mask(j->target_state, false);
                    }

                    else
                    {
                        BitSet b;
                        b.set(j->target_state);
                        //b.set(i);
                        masks[c] |= b;
                    }
                }
            }
        }

        Rs_size = errors + 1;
        Rs.resize(Rs_size);
        Rs[0].set(fsa.start_state());
        apply_epsilon(Rs[0]);


        for(int i=1; i < Rs_size; ++ i)
        {
            Rs[i] = Rs[i-1];
            Rs[i] <<= 1;
            Rs[i].set(0);
            apply_epsilon(Rs[i]);

        }

    }


    /// main search function for normal agrep search.
    int search(std::string text)
    {

        BitSet s_i;

        for(int i = 0; i < text.length() ; ++i)
        {

            unsigned char c = text[i];
            // get masks for character
            s_i = masks[c];
            // save state
            prevs = Rs;

            // shift or
            Rs[0] <<= 1;
            Rs[0] |= s_i;

            // go through error arrays
            for (int j = 1; j < Rs_size; j++)
            {
                Rs[j] <<= 1;
                Rs[j] |= ( s_i&((Rs[j-1]&prevs[j-1])<<1) & prevs[j-1]);
            }// end for j

            if (not Rs[Rs_size - 1].test(pattern_length-1))
            {
                return i - pattern_length + 1;
            } // end if
        }// end for i
        return -1;

    }// end search


    /// search using a translated thompson nfa.
    int search_fsa(std::string text)
    {
        // Rs[0] start already set
        BitSet s_i;
        //apply_epsilon(Rs[0]);


        for(int i = 0; i < text.length() ; ++i)
        {
            unsigned char c = text[i];
            s_i = masks[c];

            // std::cout<<Rs[Rs_size - 1]<<" at text: " <<text[i]<<std::endl;

            prevs = Rs;
            Rs[0].set(start);
            apply_epsilon(Rs[0]);


            Rs[0] <<= 1;
            Rs[0].set(0);


            // std::cout<<Rs[Rs_size - 1]<<" after shift " <<std::endl;
            Rs[0] &= s_i;

            // std::cout<< s_i <<" mask"<<std::endl;
            // std::cout<<Rs[Rs_size - 1]<<" after ands " <<std::endl;

            apply_epsilon(Rs[0]);

            for (int j = 1; j < Rs_size; j++)
            {
                Rs[j] <<= 1;
                Rs[j].set(0);
                Rs[j] &=( s_i|((Rs[j-1]|prevs[j-1])<<1).set(0)|prevs[j-1]);

                apply_epsilon(Rs[j],j);

            }// end for j


            // std::cout<<Rs[Rs_size - 1]<<" after jumps  " <<std::endl<<std::endl;
            if (Rs[Rs_size - 1].test(final))
            {
                for (int j = 0; j < Rs_size; j++)
                {
                    Rs[j].reset();
                }// end for j

                return i;
            }
        }
        return -1;
    }

    /// apply epsilon jumps to the whole bit array
    void apply_epsilon(BitSet &bs, unsigned offset = 0)
    {
        for (int l = 0 ; l < 2; ++l)
        {
            for(int k = 0; k < epsilons.size(); ++k)
            {
                unsigned i = epsilons[k];
                if(bs.test(i+offset))
                {
                    bs |= (transition_masks[i]<<offset);
                }
            }

        }
    }

    /// make wild card masks
    void wild_card_mask(unsigned pos, bool to_ones = true)
    {
        mask_range(pos, 0, 255, to_ones);
    }

    /// make masks for characters from range a to b.
    void mask_range(unsigned pos,unsigned  a, unsigned  b, bool to_ones = true)
    {
        // assert(int(b) > int(a));
        // assert(b<=MAX_CHAR);
        BitSet tmp_bs;
        if (to_ones)
        {
            tmp_bs.set();
            tmp_bs.flip(pos);
        }
        else
        {
            tmp_bs.set(pos);
        }


        for(unsigned k = a; k < b; ++k)
        {
            {
                // std::cout<<"HOLLA"<<std::endl;

                if (to_ones)
                {
                    masks[k] &= tmp_bs;
                }
                else
                {
                    masks[k] |= tmp_bs;

                }


                // flip according to character in pattern
            }// end if
        }
    }


    // used for making nice printy stuff
    std::string make_empty(unsigned len, unsigned pos )
    {
        std::string res;
        for (int i = 0; i < len ; ++i)
        {
            if(i == pos)
            {
                res +="^";
            }
            else
            {
                res += " ";

            }
        }
        return res;
    }



    unsigned pattern_length;

    std::vector<BitSet> masks; // mask for normal search
    std::vector<BitSet> transition_masks; // mask for epsilon jumps
    std::vector<int> epsilons; // epsilon positions
    unsigned Rs_size; // number of bit arrays of bitarrays to search
    std::vector<BitSet> Rs; // "bitmatrix" used for approximative search
    std::vector<BitSet> prevs; // temp variable
    unsigned final; // final state fsa
    unsigned start; // start state fsa.



    // deprecated member functions
    bool search_wild_card(std::string pattern, std::string text,unsigned wild_split, unsigned errors =0)
    {

        bool exact = (errors == 0);

        compile_standard(pattern);
        size_t m = pattern.length();
        unsigned Rs_size = errors + 1;
        std::vector<BitSet> Rs;
        std::vector<BitSet> prevs;

        Rs.resize(Rs_size);
        BitSet s_i;
        BitSet temp_prev;
        BitSet s_sharp;

        for(int i=1; i < Rs_size; ++ i)
        {
            Rs[i] = Rs[i-1];
            Rs[i] <<= 1;
            Rs[i].set(0);
        }// print out bit matrix



        for(unsigned i = 0; i < wild_split; ++i)
        {
            s_sharp.set(i);
        }
        std::cout<<s_sharp;



        for(int i = 0; i < text.length() ; ++i)
        {

            unsigned char c = text[i];
            s_i = masks[c];
            prevs = Rs;
            //temp_prev = Rs[0];

            Rs[0] <<= 1;
            Rs[0].set(0);

            Rs[0] &= s_i;
            //Rs[0] = Rs[0] | (Rs[0] & s_sharp);

            for (int j = 1; j < Rs_size; j++)
            {
                Rs[j] <<= 1;
                Rs[j].set(0);
                Rs[j] &=( s_i|((Rs[j-1]|prevs[j-1])<<1)|prevs[j-1]);
            }
            for (int j = 0; j < Rs_size; j++)
            {
                std::cout<<"old "<<Rs[j]<<std::endl;
                Rs[j] |= ((prevs[j-1] & s_sharp));
                std::cout<< "new " << Rs[j] <<std::endl;
            }
            if (Rs[Rs_size - 1].test(m-1)) std::cout<<"found real match at:"<< i - m + 2 <<std::endl;
        }

        return true;

    }

};
