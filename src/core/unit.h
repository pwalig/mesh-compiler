#pragma once
#include <vector>
#include "Inode.h"
#include <fstream>
#include <functional>
#include "field.h"
#include "buffer.h"
#include "printMode.h"
#include <rapidjson/document.h>
#include "formatInterpreterContext.h"

namespace mc {
	class formatInterpreterInfo;

	class unit {
	public:
		std::vector<field::ptr> preamble;
		std::vector<buffer> buffers;
		ctype::code c = ctype::null;

		unit() = default;
		unit(std::ifstream& file, formatInterpreterContext& context);
		unit(const rapidjson::Value& json);

		void output(std::ofstream& file, const Inode::ptr node, printMode pm);

		size_t getSize(const Inode::ptr node) const;
		size_t getEntriesCount(const Inode::ptr node) const;
		size_t getFieldsCount(const Inode::ptr node) const;
	};

	class fileUnit : public unit {
	public:

		std::string output_file;
		printMode mode = printMode::binary;

		fileUnit() = default;
		fileUnit(std::ifstream& file, const std::string& output_file_, formatInterpreterContext& context);
		fileUnit(const rapidjson::Value& json);

		void compile(const Inode::ptr node, bool debug = false);

		void changeName(const std::string& pattern, const std::string& newName);
		void withChangedName(
			const std::string& pattern,
			const std::string& newName,
			const std::function<void(fileUnit*)>& func
		);
	};

	namespace stype {
		template<>
		inline code getCode<mc::unit>() { return unit; }
		template<>
		inline code getCode<mc::fileUnit>() { return unit; }
	}
}