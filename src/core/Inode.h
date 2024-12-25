#pragma once
#include <string>
#include "ctype.h"
#include "vtype.h"

namespace mc {
	class Inode {
	public:
		ctype::code c;
		virtual std::string getName() const = 0;
		virtual size_t getCount() const = 0;

		virtual void writeValue(
			std::ofstream& file, vtype::code v, stype::code s,
			const std::vector<unsigned int>& suffixes = { 0 }
		) const = 0;

		virtual size_t getChildNodeCount(ctype::code counting_type) const = 0;
		virtual Inode* getChildNodeOfType(ctype::code counting_type, size_t id) const = 0;
	};
}