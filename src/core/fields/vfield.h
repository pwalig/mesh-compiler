#pragma once
#include "sfield.h"
#include <fstream>

namespace mc {
	template<typename T>
	class vfield : public sfield<T> {
	public:
		const vtype::code v;
		const std::vector<unsigned short> suffixes;

		std::vector<T> evaluate(const Inode::ptr node) const override;

		oop_ptr_template_child_define(field, vfield)
	};

	template<typename T>
	inline std::vector<T> vfield<T>::evaluate(const Inode::ptr node) const
	{
		std::vector<T> out;
		std::vector<anyType::value> vals = node->getValues(v, s, suffixes);
		for (anyType::value& v : vals) {
			out.push_back(anyType::getValue<T>(v));
		}
		return out;
	}
}