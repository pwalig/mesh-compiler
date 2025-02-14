#pragma once
#include <vector>
#include <string>

namespace mc {
	enum class printMode {
		plainText, binary
	};

	extern std::vector<std::string> plainTextSpellings;
	extern std::string binarySpelling;
}