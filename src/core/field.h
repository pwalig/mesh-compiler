#pragma once
#include "stype.h"
#include "vtype.h"
#include "Inode.h"
#include <fstream>

namespace mc {
	class field {
	public:
		const stype::code s;
		const vtype::code v;

		field(const stype::code& st, const vtype::code& vt, const void* data_source);
		field(const stype::code& st, const vtype::code& vt,
			const void* data_source, const size_t& data_amount);

		virtual void output(std::ofstream& file, const Inode* node) = 0;

	private:
		std::vector<char> data;
	};
}