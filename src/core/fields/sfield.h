#pragma once
#include "../field.h"
#include "../Inode.h"

namespace mc {
	class sfield : virtual public field {
	public:
		stype::code s;
		sfield(stype::code st = stype::null);

		void output(std::ofstream& file, const Inode::ptr node, mc::printMode pm) const override;
		virtual std::vector<anyType::value> evaluate(const Inode::ptr node) const = 0;
		size_t getSize() const override;

		oop_ptr_base_declare(field) = 0;
	};
}