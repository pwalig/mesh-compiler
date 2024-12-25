#pragma once
#include <vector>
#include "Inode.h"
#include <fstream>
#include <functional>
#include "field.h"
#include "buffer.h"


namespace mc {
	class unit {
	public:
		std::vector<field::ptr> preamble;
		std::vector<buffer> buffers;
		ctype::code c;

		void output(std::ofstream& file, const Inode* node);
	};

	class fileUnit : public unit {
	public:
		std::string output_file;

		void compile(const Inode* node);

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