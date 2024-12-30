#pragma once
#include "sfield.h"
#include <rapidjson/document.h>

namespace mc {
	template<typename T>
	class cfield : public sfield<T> {
	public:
		const T value;
		cfield(T val) : value(val) {}

		std::vector<T> evaluate(const Inode::ptr node) const override;
		ctype::code getCountingType() const override;

		oop_ptr_template_child_define(field, cfield)
	};

	field::ptr getCFieldPtr(stype::code s, anyType::value);
	field::ptr getCFieldPtr(const rapidjson::Value& json);

	template<typename T>
	inline std::vector<T> cfield<T>::evaluate(const Inode::ptr node) const
	{
		return std::vector<T>(1, value);
	}
	template<typename T>
	inline ctype::code cfield<T>::getCountingType() const
	{
		return ctype::null;
	}
}