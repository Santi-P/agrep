// Levenshtein distance
// currently not used
// SP 2.9.19

#include <vector>
#include<string>

// NOTE maybe change to size type
inline unsigned indicator(char a, char b)
{
    if (a == b) return 0;
    return 1 ;
}



unsigned levenshtein_routine(std::string w_1, std::string w_2, unsigned idx_1, unsigned idx_2)
{

    // coresponds to max(i,j) if min(i,j) = 0
    if(idx_1 == 0)
    {
        return idx_2;
    }
    else if (idx_2==0)
    {
        return idx_1;
    }

    // corresponds to min code
    unsigned curr_min = levenshtein_routine(w_1, w_2, idx_1-1, idx_2) + 1;
    unsigned candidate_min = levenshtein_routine(w_1,w_2, idx_1, idx_2 -1) + 1;

    if (curr_min > candidate_min)
    {
        curr_min = candidate_min;
    }

    candidate_min = levenshtein_routine(w_1, w_2,idx_1-1, idx_2-1) + \
                    indicator(w_1[idx_1],w_2[idx_2]);

    if(curr_min > candidate_min) return candidate_min;

    return candidate_min;
}

unsigned levenshtein(std::string w_1, std::string w_2)
{
    unsigned size_1 = w_1.length();
    unsigned size_2 = w_2.length();

    return levenshtein_routine(w_1,w_2, size_1, size_2);
}
