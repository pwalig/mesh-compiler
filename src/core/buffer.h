#pragma once
#include <vector>
#include "field.h"
#include <fstream>
#include "Inode.h"
#include "types/ctype.h"
#include <rapidjson/document.h>

namespace mc {
	class buffer {
	public:
		std::vector<field::ptr> preamble;
		std::vector<field::ptr> fields;

		ctype::code c;

		buffer() = default;
		buffer(const rapidjson::Value& json);

		void output(std::ofstream& file, const Inode::ptr node, mc::printMode pm);

		size_t getEntrySize() const;
		size_t getSize(const Inode::ptr node) const;
	};
}