#include "Inode.h"


mc::Inode::Inode(ctype::code counting_type) : c(counting_type)
{
}

std::vector<mc::anyType::value> mc::Inode::getValues(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    std::vector<mc::anyType::value> res;

    std::vector<unsigned short> maxSfxs = mc::vtype::maxSuffixes.at(v);
    if (suffixes.size() == maxSfxs.size()) {
        res.push_back(getValue(v, s, suffixes));
        return res;
    }

    std::vector<unsigned short> sfx = suffixes;
	sfx.push_back(0);
	for (unsigned short j = 0; j < maxSfxs[suffixes.size()]; ++j) {
		sfx.back() = j;
		std::vector<mc::anyType::value> va = getValues(v, s, sfx);
		res.insert(res.end(), va.begin(), va.end());
	}

    return res;
}