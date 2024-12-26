#pragma once
#include "sfield.h"
#include <fstream>

namespace mc {
	template<typename T>
	class vfield : public sfield<T> {
	public:
		const vtype::code v;
		const std::vector<unsigned short> suffixes;
		vfield(vtype::code value_type,
			const std::vector<unsigned short>& suffixes_);

		std::vector<T> evaluate(const Inode::ptr node) const override;

		oop_ptr_template_child_define(field, vfield)
	};

	template<typename T>
	inline vfield<T>::vfield(vtype::code value_type,
		const std::vector<unsigned short>& suffixes_) : v(value_type), suffixes(suffixes_)
	{
	}

	template<typename T>
	inline std::vector<T> vfield<T>::evaluate(const Inode::ptr node) const
	{
		std::vector<T> out;
		std::vector<anyType::value> vals = node->getValues(v, mc::stype::getCode<T>(), suffixes);
		for (anyType::value& v : vals) {
			out.push_back(anyType::getValue<T>(v));
		}
		return out;
	}
}