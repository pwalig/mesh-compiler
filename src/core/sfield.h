#pragma once
#include "field.h"
#include "Inode.h"

namespace mc {
	template<typename T>
	class sfield : public field {
	public:
		virtual T evaluate(const Inode* node) const = 0;

		oop_ptr_base_declare(field) = 0;
	};
}