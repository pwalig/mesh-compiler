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

		template<typename T>
		T getEntrySize() const;
		template<typename T>
		T getSize(const Inode::ptr node) const;
	};

	template<typename T>
	inline T buffer::getEntrySize() const
	{
		size_t siz = 0;
		for (const field::ptr& f : fields)
			siz += f->getSize();
		return (T)siz;
	}
	template<typename T>
	inline T buffer::getSize(const Inode::ptr node) const
	{
		return getEntrySize<T>() * (T)node->getCount();
	}
}