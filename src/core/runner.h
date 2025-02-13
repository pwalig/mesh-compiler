#pragma once
#include <string>
#include <vector>

namespace mc {
	extern const std::string version;
	extern const std::string programName;
	void run(int argc, char** argv);
	void runOnce(const std::vector<std::string>& args);
	void runOnce(int argc, char** argv);
}