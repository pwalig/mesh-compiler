#pragma once
#include "Inode.h"
#include <fstream>
#include <oop_ptr.h>
#include "types/stype.h"
#include "printMode.h"
#include <rapidjson/document.h>
#include "ctype.h"
#include "compilationContext.h"

namespace mc {
	class unit;

	class field {
	public:
		enum class location {
			main_preamble,
			buffer_preamble,
			buffer_field
		};

		virtual void output(std::ofstream& file, const Inode::ptr node, mc::printMode pm) const = 0;

		virtual mc::ctype::code getCountingType() const = 0;
		virtual size_t getSize() const = 0;

		oop_ptr_base_declare(field) = 0;
		virtual ~field() {}

		using ptr = oop_ptr<field>;

		static ptr getPtr(const rapidjson::Value& json, location loc);
		static ptr getPtr(const std::string& word, location loc, const compilationContext& context = compilationContext());
	};
}