#pragma once
#include "sfield.h"
#include "../any-type-value.h"
#include <rapidjson/document.h>

namespace mc {
	class cfield : public sfield {
	public:
		const anyType::value value;

		cfield(stype::code st, anyType::value val) : sfield(st), value(val) {}

		std::vector<anyType::value> evaluate(const Inode::ptr node) const override;
		ctype::code getCountingType() const override;

		oop_ptr_child_declare(field);

		static field::ptr getPtr(const rapidjson::Value& json);
	};
}