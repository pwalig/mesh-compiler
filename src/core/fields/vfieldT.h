#pragma once
#include "sfieldT.h"
#include <fstream>

namespace mc {
	template<typename T>
	class vfieldT : public sfieldT<T> {
	public:
		const std::vector<unsigned short> suffixes;
		const vtype::code v;

		vfieldT(vtype::code value_type,
			const std::vector<unsigned short>& suffixes_);

		std::vector<T> evaluate(const Inode::ptr node) const override;
		ctype::code getCountingType() const override;

		oop_ptr_template_child_define(field, vfieldT)
	};

	template<typename T>
	inline vfieldT<T>::vfieldT(vtype::code value_type,
		const std::vector<unsigned short>& suffixes_) : v(value_type), suffixes(suffixes_)
	{
	}

	template<typename T>
	inline std::vector<T> vfieldT<T>::evaluate(const Inode::ptr node) const
	{
		std::vector<T> out;
		std::vector<anyType::value> vals = node->getValues(v, mc::stype::getCode<T>(), suffixes);
		for (anyType::value& v : vals) {
			out.push_back(anyType::getValue<T>(v));
		}
		return out;
	}
	template<typename T>
	inline ctype::code vfieldT<T>::getCountingType() const
	{
		return vtype::ctypes.at(v);
	}
}