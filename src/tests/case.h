#pragma once
#include <functional>
#include "../core/compilation-info.h"

namespace tests {
	class Case {
	public:
		const std::function<mc::compilationInfo()> getCompilationInfo;
		const std::vector<std::string> resultingFiles;

		Case(
			const std::function<mc::compilationInfo()>& getCompilationInfo_,
			const std::vector<std::string>& resultingFiles_
		);

		static std::function<mc::compilationInfo()> getFromFile(const std::string& fileName);
	};
}