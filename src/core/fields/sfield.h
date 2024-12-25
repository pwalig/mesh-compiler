#pragma once
#include "../field.h"
#include "../Inode.h"

namespace mc {
	template<typename T>
	class sfield : virtual public field {
	public:
		void output(std::ofstream& file, const Inode::ptr node) const override;
		virtual std::vector<T> evaluate(const Inode::ptr node) const = 0;

		oop_ptr_base_declare(field) = 0;
	};
	template<typename T>
	inline void sfield<T>::output(std::ofstream& file, const Inode::ptr node) const
	{
		std::vector<T> val = evaluate(node);
		for (T& v : val) {
			file.write((char*)(&v), sizeof(T));
		}
	}
}