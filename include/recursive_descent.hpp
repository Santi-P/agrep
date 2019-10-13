// Recursive descent parser for a simple regular expression grammar
// SP 11.10.19
// contains simple tokenizer and parser.
// incrementally constructs NFA from regular expression string with no backtracking
/// heavily inspired by but not plagiarized
/// original java tutorial by matt might http://matt.might.net/articles/parsing-regex-with-recursive-descent/
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <vector>
#include "FSA.hpp"
#include "FSAWrapper.hpp"
#include <cassert>

typedef FSA<unsigned short int> FSA16;

/// Recursive descent parser class.
/// implements parser for simple regular expressions with ( , ) , | , . and *
/// based on this EBNF grammar.
///   <regex> ::= <term> '|' <regex>
///             |  <term>
///    <term> ::= { <factor> }
///    <factor> ::= <base> { '*' }
///    <base> ::= <char>
///            |  '\' <char>
///            |  '(' <regex> ')'


class RecDesc
{
public:
    /// \brief empty default construction
    RecDesc()
    {
    }
    /// \brief tokenizes the input in preparation for parsing
    /// \param in_string the string which is to be tokenized

    RecDesc(std::string in_string)
    {
        to_parse = in_string;
        tokenized_input = tokenize(in_string);
        len_inp = tokenized_input.size();

    }



    /// parses regular expression
    /// returns a Thompson Nfa FSA16 object
    FSA16 parse()
    {
        FSA16 res = regex();
        return res;
    }



private:
    std::string to_parse;
    std::vector<std::string> tokenized_input;
    unsigned len_inp;
    unsigned current_tok = 0;
    bool valid = true;


    bool is_end()
    {
        return current_tok > len_inp - 1;
    }


    /// tokenizes string into a vector of strings.
    std::vector<std::string> tokenize(std::string s)
    {

        unsigned last = 0;
        std::vector<std::string> res;
        res.reserve(s.length());
        char prev = 0;
        for(int i=0; i<s.length() ; ++i)
        {

            char curr = s[i];

            if (curr == '|' || curr =='*' || curr =='(' ||  curr == ')')
            {
                std::string tmp_string = "";
                tmp_string += curr;
                if(i > 0) prev = s[i-1];
                if (prev == 0)
                {
                    //
                }
                else if (not (prev == '|' || prev =='*' || prev =='(' ||  prev == ')'))
                {
                    res.push_back(s.substr(last, i-last));

                }

                res.push_back(tmp_string);
                last = i + 1;
            }

        }

        if (last != s.length())
        {
            res.push_back(s.substr(last,s.length() ));
        }
        return res;
    }

    // deprecated tokenizer
    std::vector<std::string> shitty_tokenize(std::string s)
    {
        std::vector<std::string> res;
        for(int i = 0; i < s.length(); i++)
        {
            std::string tmp;
            tmp += s[i];

            res.push_back(tmp);
        }
        return res;
    }

    // helper functions

    std::string peek()
    {
        if (is_end())
        {
            return "";
        }
        return tokenized_input[current_tok];
    }

    std::string next()
    {
        //assert(current_tok < len_inp);
        std::string res = tokenized_input[current_tok ];
        current_tok += 1;
        return res;
    }

    // this is where reject happens
    bool eat(std::string s)
    {
        if (s == tokenized_input[current_tok])
        {
            current_tok += 1;
            return true;
        }

        else
        {
            std::cerr << "Error: Invalid Syntax"<<std::endl;
            exit(2);
        }

        return false;
    }



    // grammar functions
    FSA16 regex()
    {

        FSA16 res =  term();
        if ( (not is_end() ) and peek() == "|")
        {
            eat("|");
            FSA16 rhs = regex();
            return fsa_union(res, rhs);
        }

        return res;
    }

    FSA16 term()
    {
        FSA16 res = factor();
        while( (not is_end()) and peek() != ")" and peek() != "|")
        {
            FSA16 next_fac = factor();
            res = fsa_concat(res,next_fac);
        }
        return res;
    }

    FSA16 factor()
    {
        // check for closure
        FSA16 res = base();
        while ((not is_end() ) and (peek() =="*"))
        {
            res = fsa_closure(res);

            eat("*");
        }
        return res ;

    }

    FSA16 base()
    {
        if (peek() == "(")
        {
            eat("(");
            FSA16 res = regex();
            eat(")");
            return res;
        }
        else if (peek() ==")")
        {
            std::cerr << "Error: Invalid Syntax"<<std::endl;
        }

        FSA16 fsa(next());
        return fsa;
    }
};

