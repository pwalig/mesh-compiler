#pragma once
#include "Inode.h"
#include <fstream>
#include <oop_ptr.h>
#include "types/stype.h"

namespace mc {
	class unit;

	class field {
	public:
		stype::code s;

		virtual void output(std::ofstream& file, const Inode::ptr node) const = 0;

		template<typename T>
		T getSize() const;

		oop_ptr_base_declare(field) = 0;

		using ptr = oop_ptr<field>;
	};

	template<typename T>
	inline T field::getSize() const
	{
		return stype::sizes.at(s);
	}
}