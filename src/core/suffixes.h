#pragma once
#include <vector>
#include <string>
#include <unordered_map>

namespace mc {
	extern std::unordered_map<char, unsigned short> suffixes;
	std::vector<unsigned short> extractSuffixes(std::string& vtype_plus);
}
