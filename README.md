# agrep
Fuzzy Regular Expression|String Search by Santi Pornavalai

## Description
Simple Fuzzy Search using Shift OR/AND Algorithm loosely based on the algorithm described by (Wu&Manber 1992). 

## Installation
simply run **make run** or **make all** to compile the project. This code was tested using g++ -O2 for the most part. 
The only dependencies aside aside from STL is boost/program_options and the FSA class from CPII2019 course. 
**make test** would build the project and test it against randomized input as well as comparing its results to the standard grep. 
**make document** generates documentations using doxygen 

## Usage
*SYNOPSIS: agrep [-e] [--help --expression] [-k <number>] <PATTERN> <FILENAME>*
- **-e** or **--expression** toggles regular expression mode. With this option selected, the <PATTERN> is interpreted as a regular expression. The syntax used is a simplified version of POSIX containing only:
  - ( ) 
  - | for union
  - \* for 0 or more repitions
  - . for wild card  
- **-k** followed by an integer specifies the edit distance (allowed errors). 
- **--help** prints out a short synopsis of the arguments.  

## Notes
According to Wu & Manber, the main advantage of the algorithm stems from the fact, that it is based around a few efficient operations. The trade off of using this approach is that, without further tricks, the maximum
pattern length is capped to the computer's word size (in my case 64). It is possible it may overflow a on shorter pattern when using regular expression. This is because Epsilon transitions also take up 1 bit.
Nevertheless when this happens, the program would print out a error and exit. std::bitset is used under the hood. The reason for using std::bitset over boost::dynamic_bitset is that based on my few tests, dynamic_bitset has proven to be rather slow.  
Additionally, I have found out that the performance gained from using shift-and instead of shift-or and the flipping the firste bit was negligeable. Another difference between this implementation and the original is the handling of epsilon transitions.
WU & Manber's version used the strategy of dividing the bit array into bytes and looking them up to determine when to "jump". My version simply remembers the epsilon states and applies the jumps accordingly. This is less efficient but it is somewhat difficult to 
to divide bitset into bytes. One strategy of doing this with bitset is to make a copy and repeatedly shift 4 and casting it as char.  

Compared to grep, my implementation of agrep is around 5-10X slower. The reason for this being that I'm not Ken Thompson. Nevertheless it is still a lot faster than the original open-sourced implementation of agrep. 

## Bugs and TODO
- Implement character classes. The backend functions are basically there already. The tokenizer just needs to be improved. 
- Maybe rework epsilon jumps and modify the FSA class to produce less epsilon jumps... -> minimize maybe? 
- I forgot to allow it to continue to search the line when a pattern is found.
- There is a bug that sometimes messes up the first character when using regular expression. I can't really find it and I'm tired. 


## References
Wu, S., & Manber, U. (1992). Fast Text Searching Allowing Errors. Commun. ACM, 35, 83-91.
