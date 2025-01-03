#include "ufield.h"
#include "../unit.h"

void mc::ufield::output(std::ofstream& file, const Inode::ptr node, printMode pm) const
{
	unitsMap->at(unitName).output(file, node, pm);
}

mc::ctype::code mc::ufield::getCountingType() const
{
	return unitsMap->at(unitName).c;
}

size_t mc::ufield::getSize() const
{
	throw std::logic_error("getting ufield size requires a node");
}

oop_ptr_define(mc::field, mc::ufield)
