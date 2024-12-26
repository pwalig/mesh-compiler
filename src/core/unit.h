#pragma once
#include <vector>
#include "Inode.h"
#include <fstream>
#include <functional>
#include "field.h"
#include "buffer.h"
#include "printMode.h"
#include <rapidjson/document.h>


namespace mc {
	class unit {
	public:
		std::vector<field::ptr> preamble;
		std::vector<buffer> buffers;
		ctype::code c;

		unit() = default;
		unit(const rapidjson::Value& json);

		void output(std::ofstream& file, const Inode::ptr node, printMode pm);

		template<typename T>
		T getSize(const Inode::ptr node) const;
		template<typename T>
		T getEntriesCount(const Inode::ptr node) const;
		template<typename T>
		T getFieldsCount(const Inode::ptr node) const;
	};

	class fileUnit : public unit {
	public:

		std::string output_file;
		printMode mode = printMode::binary;

		fileUnit() = default;
		fileUnit(const rapidjson::Value& json);

		void compile(const Inode::ptr node);

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

	template<typename T>
	inline T unit::getSize(const Inode::ptr node) const
	{
		T siz = 0;
		for (const buffer& buff : buffers)
			siz += buff.getSize(node->getChildNodeOfType(buff.c));
		return siz;
	}
	template<typename T>
	inline T unit::getEntriesCount(const Inode::ptr node) const
	{
		T siz = 0;
		for (const buffer& buff : buffers)
			siz += node->getChildNodeCount(buff.c);
		return siz;
	}
	template<typename T>
	inline T unit::getFieldsCount(const Inode::ptr node) const
	{
		T siz = 0;
		for (const buffer& buff : buffers)
			siz += buff.fields.size() * node->getChildNodeCount(buff.c);
		return siz;
	}
}