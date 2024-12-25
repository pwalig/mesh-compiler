#pragma once
#include <unordered_map>
#include <string>
#include "unit.h"

namespace mc {
	class compilationInfo {
		std::unordered_map<std::string, unit> units;
		std::vector<fileUnit> file_units;

		void compileFile(const std::string& filename);
	};
}