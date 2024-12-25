#pragma once
#include "sfield.h"

namespace mc {
	template<typename T>
	class vfield : public sfield<T> {
	public:
		const vtype::code v;
		const std::vector<unsigned short> suffixes;

		void output(std::ofstream& file, const Inode* node) const override;
		T evaluate(const Inode* node) const override;
	};

	template<typename T>
	inline void vfield<T>::output(std::ofstream& file, const Inode* node) const
	{
		node->writeValue(file, v, stype::getCode<T>(), suffixes);
	}

	template<typename T>
	inline T vfield<T>::evaluate(const Inode* node) const
	{
		return anyType::getValue<T>(node->getValue(v, stype::getCode<T>(), suffixes));
	}
}