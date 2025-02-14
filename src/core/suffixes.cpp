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
    {'0', 0},
    {'1', 1},
    {'2', 2},
    {'3', 3},
    {'4', 4},
    {'5', 5},
    {'6', 6},
    {'7', 7},
    {'8', 8},
    {'9', 9}
};

std::string mc::vtypeNoSuffix(const std::string& vtype_w_sfx)
{
    auto pos = vtype_w_sfx.find_first_of('.');
    return vtype_w_sfx.substr(0, pos);
}

std::vector<unsigned short> mc::extractSuffixes(const std::string& vtype_w_suff)
{
    std::vector<unsigned short> res;
    for (size_t i = 0; i < vtype_w_suff.size(); ++i) {
        if (vtype_w_suff[i] == '.') {
            ++i;
            if (vtype_w_suff.size() < i + 1) throw std::runtime_error("suffix expected after .");
            res.push_back(suffixes.at(vtype_w_suff[i]));
        }
    }
    return res;
}
