#pragma once
#include "sfield.h"
#include <fstream>

namespace mc {
	template<typename T>
	class vfield : public sfield<T> {
	public:
		const vtype::code v;
		const std::vector<unsigned short> suffixes;

		void output(std::ofstream& file, const Inode* node, const unit* u) const override;
		T evaluate(const Inode* node, const unit* u) const override;

		oop_ptr_template_child_define(field, vfield);
	};

	template<typename T>
	inline void vfield<T>::output(std::ofstream& file, const Inode* node, const unit* u) const
	{
		T val = evaluate(node, u);
		file.write((char*)(&val), sizeof(T));
	}

	template<typename T>
	inline T vfield<T>::evaluate(const Inode* node, const unit* u) const
	{
		return anyType::getValue<T>(node->getValue(v, stype::getCode<T>(), suffixes));
	}
}