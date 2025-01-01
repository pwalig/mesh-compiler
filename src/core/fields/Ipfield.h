#pragma once
#include "../types/ptype.h"
#include "../buffer.h"
#include "../unit.h"

namespace mc {

	class Ibpfield : virtual public field {
	public:
		const buffer* buff;
		const ptype::code p;

		Ibpfield(ptype::code pt) : p(pt), buff(nullptr) {}
		oop_ptr_base_declare(field) = 0;
	};

	class Ipfield : virtual public field {
	public:
		const ptype::code p;
		const unit* u;
		const buffer* buff;
		Ipfield(ptype::code pt) : p(pt), u(nullptr), buff(nullptr) {}
		oop_ptr_base_declare(field) = 0;
	};
}
