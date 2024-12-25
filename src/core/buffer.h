#pragma once
#include <vector>
#include "field.h"
#include <fstream>
#include "Inode.h"

namespace mc {
	class buffer {
	public:
		std::vector<field*> preamble;
		std::vector<field*> fields;

		ctype::code c;

		void output(std::ofstream& file, const Inode* node);
	};
}