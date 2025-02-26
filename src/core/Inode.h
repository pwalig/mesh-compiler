#pragma once
#include <string>
#include "ctype.h"
#include "types/vtype.h"
#include "types/stype.h"
#include "any-type-value.h"
#include <oop_ptr.hpp>

namespace mc {
	class Inode {
	public:
		using ptr = oop_ptr<Inode>;

		const ctype::code c;

		Inode(ctype::code counting_type);
		virtual std::string getName() const = 0;
		virtual size_t getCount() const = 0;

		virtual anyType::value getValue(
			vtype::code v, stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const = 0;

		std::vector<anyType::value> getValues(
			vtype::code v, stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const;

		virtual size_t getChildNodeCount(ctype::code counting_type) const = 0;
		virtual ptr getChildNodeOfType(ctype::code counting_type, size_t id) const = 0;

		oop_ptr_base_declare(Inode) = 0;
	};
}