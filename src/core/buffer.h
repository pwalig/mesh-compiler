#pragma once
#include <vector>
#include "field.h"

namespace mc {
	class buffer {
		std::vector<field*> preamble;
		std::vector<field*> fields;

		ctype::code c;
	};
}