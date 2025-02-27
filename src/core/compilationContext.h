#pragma once

namespace mc {
	struct compilationContext {
		std::string filename = "";
		bool debug = false;
		bool thread = false;
	};
}
