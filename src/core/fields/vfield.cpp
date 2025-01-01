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

mc::field::ptr mc::vfield::getPtr(const rapidjson::Value& json)
{
	assert(json.IsObject());
	assert(json.HasMember("value"));
	assert(json.HasMember("type"));
	assert(json["value"].IsString());
	assert(json["type"].IsString());

	std::string vtype_str(json["value"].GetString());
	auto suffixes = mc::extractSuffixes(vtype_str);

	return field::ptr(new vfield(stype::codes.at(json["type"].GetString()), vtype::codes.at(vtype_str), suffixes));
}

oop_ptr_define(mc::field, mc::vfield)