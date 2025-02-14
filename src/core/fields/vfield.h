#pragma once
#include "sfield.h"
#include <fstream>

namespace mc {
	class vfield : public sfield {
	public:
		const std::vector<unsigned short> suffixes;
		const vtype::code v;

		vfield(stype::code s, vtype::code value_type,
			const std::vector<unsigned short>& suffixes_);

		std::vector<anyType::value> evaluate(const Inode::ptr node) const override;
		ctype::code getCountingType() const override;

		oop_ptr_child_declare(field);

		static field::ptr getPtr(const rapidjson::Value& json);
		static bool gettable(const std::string& word);
	};
}