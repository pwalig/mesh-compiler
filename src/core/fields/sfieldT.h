#pragma once
#include "../field.h"
#include "../Inode.h"

namespace mc {
	template<typename T>
	class sfieldT : virtual public field {
	public:
		void output(std::ofstream& file, const Inode::ptr node, mc::printMode pm) const override;
		virtual std::vector<T> evaluate(const Inode::ptr node) const = 0;

		oop_ptr_base_declare(field) = 0;

	private:
		size_t getSize1() const override;
	};
	template<typename T>
	inline void sfieldT<T>::output(std::ofstream& file, const Inode::ptr node, mc::printMode pm) const
	{
		std::vector<T> val = evaluate(node);
		for (T& v : val) {
			if (pm == printMode::binary) file.write((char*)(&v), sizeof(T));
			else file << v << " ";
		}
	}
	template<typename T>
	inline size_t sfieldT<T>::getSize1() const
	{
		return sizeof(T);
	}
}