#pragma once
#include <string>
#include "ctype.h"
#include "types/vtype.h"
#include "types/stype.h"
#include "any-type-value.h"
#include <oop_ptr.h>

namespace mc {
	class Inode {
	public:
		using ptr = oop_ptr<Inode>;

		const ctype::code c;

		Inode(ctype::code counting_type);
		virtual std::string getName() const = 0;
		virtual size_t getCount() const = 0;

		virtual anyType::value getValue(
			vtype::code v, stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const = 0;

		std::vector<anyType::value> getValues(
			vtype::code v, stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const;

		virtual size_t getChildNodeCount(ctype::code counting_type) const = 0;
		virtual ptr getChildNodeOfType(ctype::code counting_type, size_t id) const = 0;

		oop_ptr_base_declare(Inode) = 0;
	};
}

#define InodeTemplate(name, ...)																	\
	class name : public mc::Inode {																	\
	public:																							\
		__VA_ARGS__																					\
																									\
		std::string getName() const override;														\
		size_t getCount() const override;															\
																									\
		mc::anyType::value getValue(																\
			mc::vtype::code v, mc::stype::code s,													\
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()				\
		) const override;																				\
																										\
		size_t getChildNodeCount(mc::ctype::code counting_type) const;									\
		mc::Inode::ptr getChildNodeOfType(mc::ctype::code counting_type, size_t id) const override;		\
																										\
		oop_ptr_child_declare(mc::Inode, name);															\
	}																									\