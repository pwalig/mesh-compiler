#pragma once
#include <vector>
#include <string>
#include "case.h"

namespace tests {
	void run(
		const std::vector<Case> cases,
		const std::vector<std::string>& refereceFiles,
		const std::string& sourceFile
	);

	void run();
}