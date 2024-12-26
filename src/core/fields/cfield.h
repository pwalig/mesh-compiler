#pragma once
#include "sfield.h"

namespace mc {
	template<typename T>
	class cfield : public sfield<T> {
	public:
		const T value;
		cfield(T val) : value(val) {}

		std::vector<T> evaluate(const Inode::ptr node) const override;

		oop_ptr_template_child_define(field, cfield)
	};

	template<typename T>
	inline std::vector<T> cfield<T>::evaluate(const Inode::ptr node) const
	{
		return std::vector<T>(1, value);
	}
}