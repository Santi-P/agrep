
#include <vector>
#include <string> 
#include <bitset> 



class BitPattern{
    public:
    BitPattern(std::string pattern){
    masks.reserve(hi);

    }

    private:

    void compile(std::string pattern){
        auto len = pattern.length();
        std::bitset<32> b;

        // replace 32 with systems register width

        std::vector<std::bitset<32>> masks ;

        for (int i = 0; i < hi; ++i){
            masks.push_back(b);
        }

        for (unsigned j = 0; j < len; ++j){ 

            //
            unsigned char c = pattern[j];
            if (masks[c].any()) continue; 

            std::bitset<32> tmp_bs;

            for(unsigned k = 0; k < len; ++k){
                if (c == pattern[k]){
                    tmp_bs.set(k);
                }
            }

            masks[c] = tmp_bs;

            }
    }
    
    const size_t hi = std::numeric_limits<unsigned char>::max();

    std::vector<std::bitset<32>> masks ;



};


