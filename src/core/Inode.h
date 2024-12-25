#pragma once
#include <string>
#include "ctype.h"
#include "types/vtype.h"
#include "types/stype.h"
#include "any-type-value.h"

namespace mc {
	class Inode {
	public:
		ctype::code c;
		virtual std::string getName() const = 0;
		virtual size_t getCount() const = 0;

		virtual anyType::value getValue(
			vtype::code v, stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const = 0;

		virtual std::vector<anyType::value> getValues(
			vtype::code v, stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const = 0;

		virtual size_t getChildNodeCount(ctype::code counting_type) const = 0;
		virtual Inode* getChildNodeOfType(ctype::code counting_type, size_t id) const = 0;
	};
}