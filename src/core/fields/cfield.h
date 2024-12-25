#pragma once
#include "sfield.h"

namespace mc {
	template<typename T>
	class cfield : public sfield<T> {
	public:
		const T value;

		std::vector<T> evaluate(const Inode::ptr node) const override;

		oop_ptr_template_child_define(field, vfield)
	};

	template<typename T>
	inline std::vector<T> cfield<T>::evaluate(const Inode::ptr node) const
	{
		return std::vector<T>(1, value);
	}
}