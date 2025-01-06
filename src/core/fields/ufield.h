#pragma once
#include "../field.h"

namespace mc {
	class compilationInfo;

	class ufield : public field {
	public:
		const std::string unitName;

		ufield(const std::string& unitName_);

		void output(std::ofstream& file, const Inode::ptr node, printMode pm) const override;

		mc::ctype::code getCountingType() const override;
		size_t getSize() const override;

		oop_ptr_child_declare(field);
	};
}