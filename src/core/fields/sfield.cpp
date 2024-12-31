#include "sfield.h"

void mc::sfield::output(std::ofstream& file, const Inode::ptr node, mc::printMode pm) const
{
	std::vector<anyType::value> val = evaluate(node);
	if (pm == printMode::binary) {
		for (anyType::value& v : val) anyType::outputBinary(v, s, file);
	}
	else {
		for (anyType::value& v : val) {
			anyType::output(v, s, file);
			file << " ";
		}
	}
}

size_t mc::sfield::getSize1() const
{
	return stype::sizes.at(s);
}
