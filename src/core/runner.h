#pragma once
#include <string>
#include <vector>

namespace mc {
	extern const std::string version;
	void run(int argc, char** argv);
	void runOnce(const std::vector<std::string>& args);
}