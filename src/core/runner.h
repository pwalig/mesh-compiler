#pragma once
#include <string>
#include <vector>

namespace mc {
	extern const std::string version;
	extern const std::string programName;
	void run(int argc, char** argv);
	void runOnce(int argc, char** argv);
}