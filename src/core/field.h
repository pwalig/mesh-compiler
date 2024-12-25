#pragma once
#include "stype.h"
#include "vtype.h"

namespace mc {
	class field {
	public:
		const stype::code s;
		const vtype::code v;

		field(const stype::code& st, const vtype::code& vt, const void* data_source);
		field(const stype::code& st, const vtype::code& vt,
			const void* data_source, const size_t& data_amount);

	private:
		std::vector<char> data;
	};
}