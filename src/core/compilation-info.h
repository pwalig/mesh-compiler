#pragma once
#include <unordered_map>
#include <string>
#include "unit.h"
#include <rapidjson/document.h>

namespace mc {
	class compilationInfo {
	public:
		std::unordered_map<std::string, unit> units;
		std::vector<fileUnit> file_units;

		compilationInfo() = default;
		compilationInfo(const std::string& filename);

		void compileFile(const std::string& filename);
	};
}