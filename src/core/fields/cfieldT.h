#pragma once
#include "sfieldT.h"
#include <rapidjson/document.h>

namespace mc {
	template<typename T>
	class cfieldT : public sfieldT<T> {
	public:
		const T value;

		cfieldT(T val) : value(val) {}

		std::vector<T> evaluate(const Inode::ptr node) const override;
		ctype::code getCountingType() const override;

		oop_ptr_template_child_define(field, cfieldT)
	};

	field::ptr getCFieldPtr(stype::code s, anyType::value);
	field::ptr getCFieldPtr(const rapidjson::Value& json);

	template<typename T>
	inline std::vector<T> cfieldT<T>::evaluate(const Inode::ptr node) const
	{
		return std::vector<T>(1, value);
	}
	template<typename T>
	inline ctype::code cfieldT<T>::getCountingType() const
	{
		return ctype::null;
	}
}