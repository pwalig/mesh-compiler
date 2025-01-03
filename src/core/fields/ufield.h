#pragma once
#include "../field.h"

namespace mc {
	class ufield : public field {
	public:
		const std::string unitName;
		std::unordered_map<std::string, unit>* unitsMap;

		void output(std::ofstream& file, const Inode::ptr node, printMode pm) const override;

		mc::ctype::code getCountingType() const override;
		size_t getSize() const override;

		oop_ptr_child_declare(field);
	};
}