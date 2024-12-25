#pragma once
#include "sfield.h"

namespace mc {
	template<typename T>
	class pfield : public sfield<T> {
	public:
		const vtype::code v;

		void output(std::ofstream& file, const Inode* node) const override;
		T evaluate(const Inode* node) const override;
	};
	template<typename T>
	inline void pfield<T>::output(std::ofstream& file, const Inode* node) const
	{
	}
	template<typename T>
	inline T pfield<T>::evaluate(const Inode* node) const
	{
		return T();
	}
}