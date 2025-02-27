#include "sfield.h"
#include "../../endian.h"

mc::sfield::sfield(stype::code st) : s(st) {}

void mc::sfield::output(std::ofstream& file, const Inode::ptr node, mc::printMode pm) const
{
	std::vector<anyType::value> val = evaluate(node);
	if (pm == printMode::plainText) {
		for (anyType::value& v : val) {
			anyType::output(v, s, file);
			file << " ";
		}
	}
	else if (pm == printMode::littleEndian && endian::host() == endian::ness::big ||
		pm == printMode::bigEndian && endian::host() == endian::ness::little) {
		for (anyType::value& v : val) {
			anyType::value v2 = anyType::getWithSwappedBytes(v, s);
			file.write((char*)(&v2), stype::sizes.at(s));
		}
	}
	else {
		for (anyType::value& v : val) file.write((char*)(&v), stype::sizes.at(s));
	}
}

size_t mc::sfield::getSize() const
{
	return stype::sizes.at(s);
}
