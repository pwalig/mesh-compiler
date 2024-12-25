#pragma once
#include "sfield.h"
#include "../types/ptype.h"

namespace mc {
	template<typename T>
	class pfield : public sfield<T> {
	public:
		const ptype::code p;

		void output(std::ofstream& file, const Inode* node, const unit* u) const override;
		T evaluate(const Inode* node) const override;

		oop_ptr_template_child_define(field, pfield)
	};
	template<typename T>
	inline void pfield<T>::output(std::ofstream& file, const Inode* node, const unit* u) const
	{
	}
	template<typename T>
	inline T pfield<T>::evaluate(const Inode* node) const
	{
		return T();
	}
}