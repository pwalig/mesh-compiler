#pragma once
#include <vector>
#include <string>
#include <unordered_map>

namespace mc {
	extern std::unordered_map<char, unsigned short> suffixes;
	std::string vtypeNoSuffix(const std::string& vtype_w_sfx);
	std::vector<unsigned short> extractSuffixes(const std::string& vtype_w_sfx);
}
