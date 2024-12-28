#pragma once
#include "Inode.h"
#include <fstream>
#include <oop_ptr.h>
#include "types/stype.h"
#include "printMode.h"
#include <rapidjson/document.h>
#include "ctype.h"

namespace mc {
	class unit;

	class field {
	public:
		virtual void output(std::ofstream& file, const Inode::ptr node, mc::printMode pm) const = 0;

		template<typename T>
		T getSize() const;

		virtual mc::ctype::code getCountingType() const = 0;

		oop_ptr_base_declare(field) = 0;
		virtual ~field() {}

		using ptr = oop_ptr<field>;

		static ptr getPtr(const rapidjson::Value& json);

	private:
		virtual size_t getSize1() const = 0;
	};

	template<typename T>
	inline T field::getSize() const
	{
		return (T)getSize1();
	}
}