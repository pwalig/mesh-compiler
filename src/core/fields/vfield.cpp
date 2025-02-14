#include "vfield.h"
#include "../suffixes.h"

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

size_t mc::vfield::getSize() const
{
	return this->sfield::getSize() * getCount();
}

size_t mc::vfield::getCount() const
{
	size_t count = 1;
    const std::vector<unsigned short> maxSfxs = mc::vtype::maxSuffixes.at(v);
    for (int i = (int)maxSfxs.size() - 1; i >= (int)suffixes.size(); --i)
    {
		count *= maxSfxs[i];
    }

	return count;
}

mc::field::ptr mc::vfield::getPtr(const rapidjson::Value& json)
{
	assert(json.IsObject());
	assert(json.HasMember("value"));
	assert(json.HasMember("type"));
	assert(json["value"].IsString());
	assert(json["type"].IsString());

	std::string vtype_str(json["value"].GetString());
	auto suffixes = mc::extractSuffixes(vtype_str);
	vtype_str = vtypeNoSuffix(vtype_str);
	assert(vtype::codes.find(vtype_str) != vtype::codes.end());

	return field::ptr(new vfield(stype::codes.at(json["type"].GetString()), vtype::codes.at(vtype_str), suffixes));
}

bool mc::vfield::gettable(const std::string& word)
{
	size_t pos = word.find_first_of(':'); // type separator
	std::string valuestr = word;
	if (pos != std::string::npos) { // TO DO check if stype specifier is valid
		valuestr = word.substr(pos + 1, word.size() - pos);
	}
	std::string vtype_str = vtypeNoSuffix(valuestr);
	if (vtype::codes.find(vtype_str) != vtype::codes.end()) {
		return true;
	}
	return false;
}

oop_ptr_define(mc::field, mc::vfield)