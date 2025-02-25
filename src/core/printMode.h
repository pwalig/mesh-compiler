#pragma once
#include <vector>
#include <string>

namespace mc {
	enum class printMode {
		plainText, binary, littleEndian, bigEndian
	};

	extern std::vector<std::string> plainTextSpellings;
	extern std::string binarySpelling;
	extern std::vector<std::string> littleEndianSpellings;
	extern std::vector<std::string> bigEndianSpellings;
}