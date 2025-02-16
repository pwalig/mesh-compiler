#pragma once
#include <vector>
#include "field.h"
#include <fstream>
#include "Inode.h"
#include "types/ctype.h"
#include <rapidjson/document.h>
#include <functional>

namespace mc {
	class compilationInfo;

	class buffer {
	public:
		std::vector<field::ptr> preamble;
		std::vector<field::ptr> fields;

		ctype::code c = ctype::null;

		buffer() = default;
		buffer(const rapidjson::Value& json, bool sizeQuerry);

		void output(std::ofstream& file, const Inode::ptr node, mc::printMode pm);

		size_t getEntrySize() const;
		size_t fieldsPerEntry() const;
		size_t getSize(const Inode::ptr node) const;
	};

	void updateCtype(ctype::code& current, ctype::code new_,
		std::function<void(ctype::code new_, ctype::code old)> conflictCheck
	);
}