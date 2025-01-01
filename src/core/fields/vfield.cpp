#include "vfield.h"

mc::vfield::vfield(stype::code s, vtype::code value_type,
	const std::vector<unsigned short>& suffixes_) : sfield(s), v(value_type), suffixes(suffixes_)
{
}

std::vector<mc::anyType::value> mc::vfield::evaluate(const Inode::ptr node) const
{
	return node->getValues(v, s, suffixes);
}

mc::ctype::code mc::vfield::getCountingType() const
{
	return vtype::ctypes.at(v);
}

oop_ptr_define(mc::field, mc::vfield)