#pragma once
#include "stype.h"
#include "vtype.h"
#include "Inode.h"
#include <fstream>
#include <oop_ptr.h>

namespace mc {
	class field {
	public:
		virtual void output(std::ofstream& file, const Inode* node) const = 0;

		oop_ptr_base_declare(field) = 0;

		using ptr = oop_ptr<field>;
	};
}