#pragma once
#include "sfield.h"
#include "../buffer.h"
#include "../unit.h"
#include "../types/ptype.h"

namespace mc {
	class bpfield : public sfield {
	public:
		const buffer* buff;
		const ptype::code p;

		bpfield(stype::code st, ptype::code pt) : sfield(st), p(pt), buff(nullptr) {}

		std::vector<anyType::value> evaluate(const Inode::ptr node) const override;
		std::vector<anyType::value> evaluate1(const Inode::ptr node, const buffer* buffe) const;
		ctype::code getCountingType() const override;

		oop_ptr_child_declare(field);
	};


	class pfield : public bpfield {
	public:
		const unit* u;

		pfield(stype::code st, ptype::code pt) : bpfield(st, pt), u(nullptr) {}

		std::vector<anyType::value> evaluate(const Inode::ptr node) const override;
		ctype::code getCountingType() const override;

		oop_ptr_child_declare(field);
	};
}
