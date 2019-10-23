// driver code for Agrep
// SP 13.10.19

#include <iostream>
#include "agrep.hpp"
#include "FSA.hpp"
#include "FSAWrapper.hpp"
#include <boost/program_options.hpp>
#include <string>
#include <iterator>
//#include "recursive_descent.hpp"
//#include "levenshtein.hpp"


namespace po = boost::program_options;

bool regex = false;
bool colors = true;
unsigned errors = 0;
std::string help_string = " agrep [OPTIONS] <PATTERN> <FILENAME> \n For more Info use option --help \n";
std::string manual = " SYNOPSIS: agrep [-e] [-k <number>][--no-colors] <PATTERN> <FILENAME> \n";

int main(int argc,const char* argv[])
{


    try
    {
        po::options_description description("Allowed Options");

        description.add_options()
        ("help,h", "quick help")
        ("no-colors", "don't output pretty colors")
        ("edit,k",po::value<unsigned>(), "edit distance for fuzzy search")
        ("expression,e","regular expression mode search");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv,description),vm);
        po::notify(vm);



        if (vm.count("help"))
        {
            std::cout <<manual;
            exit(0);
        }

        if (vm.count("no-colors"))
        {
            colors = false;
        }

        if (vm.count("edit"))
        {
            errors = vm["edit"].as<unsigned>() ;
        }


        if (vm.count("expression"))
        {
            regex = true;
        }
    }
    catch(po::error& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        std::cerr << help_string;
        exit(2);
    }

    catch(...)
    {
        std::cerr << "Unknown Error" <<std::endl;
        exit(2);
    }


    if(argc < 2)
    {
        std::cerr<< "Error: Invalid Number of Arguments"<<std::endl;
        std::cerr << help_string <<std::endl;
        exit(2);
    }


    if(argc > 6)
    {
        std::cerr<< "Error: Invalid Number of Arguments"<<std::endl;
        std::cerr << help_string <<std::endl;
        exit(2);
    }

    std::string pattern = argv[argc-2];
    std::string f_name = argv[argc-1];

    Agrep A;

// Agrep A(pattern, f_name, errors, regex );

    A.search_file(pattern, f_name,errors, regex,colors);



// copy and assignment constructor test.
// if uncommented should print results 3 times.

// Agrep B(A);
// B.search_file(pattern, f_name,errors, regex );
// Agrep C;
// C = B;
// C.search_file(pattern, f_name,errors, regex );

    exit(0);

}
