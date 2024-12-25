#pragma once
#include <vector>
#include "field.h"
#include <fstream>
#include "Inode.h"
#include "types/ctype.h"

namespace mc {
	class buffer {
	public:
		std::vector<field::ptr> preamble;
		std::vector<field::ptr> fields;

		ctype::code c;

		void output(std::ofstream& file, const Inode* node, const unit* u);

		template<typename T>
		T getEntrySize() const;
		template<typename T>
		T getSize(T count) const;
	};

	template<typename T>
	inline T buffer::getEntrySize() const
	{
		T siz = 0;
		for (const field::ptr& f : fields)
			siz += f->getSize<T>();
		return siz;
	}
	template<typename T>
	inline T buffer::getSize(T count) const
	{
		return getEntrySize<T>() * count;
	}
}