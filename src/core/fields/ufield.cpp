#include "ufield.h"
#include "../unit.h"
#include "../compilation-info.h"

mc::ufield::ufield(const std::string& unitName_) :
	unitName(unitName_) { }

void mc::ufield::output(std::ofstream& file, const Inode::ptr node, printMode pm) const
{
	mc::compilationInfo::units.at(unitName).output(file, node, pm);
}

mc::ctype::code mc::ufield::getCountingType() const
{
	return mc::compilationInfo::units.at(unitName).c;
}

size_t mc::ufield::getSize() const
{
	throw std::logic_error("getting ufield size requires a node");
}

oop_ptr_define(mc::field, mc::ufield)
