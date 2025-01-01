#include "suffixes.h"
#include <stdexcept>

std::unordered_map<char, unsigned short> mc::suffixes = {
    {'x', 0},
    {'y', 1},
    {'z', 2},
    {'w', 3},
    {'r', 0},
    {'g', 1},
    {'b', 2},
    {'a', 3},
    {'1', 0},
    {'2', 1},
    {'3', 2},
    {'4', 3},
    {'5', 4},
    {'6', 5},
    {'7', 6},
    {'8', 7},
    {'9', 8}
};

std::vector<unsigned short> mc::extractSuffixes(std::string& vtype_plus)
{
    std::vector<unsigned short> res;
    for (size_t i = 0; i < vtype_plus.size(); ++i) {
        if (vtype_plus[i] == '.') {
            ++i;
            if (vtype_plus.size() < i + 1) throw std::runtime_error("suffix expected after .");
            res.push_back(suffixes.at(vtype_plus[i]));
        }
    }
    return res;
}
