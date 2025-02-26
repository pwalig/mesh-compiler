#include "cfield.h"

std::vector<mc::anyType::value> mc::cfield::evaluate(const Inode::ptr node) const
{
    return std::vector<anyType::value>(1, value);
}

mc::ctype::code mc::cfield::getCountingType() const
{
    return ctype::null;
}

oop_ptr_define(mc::field, mc::cfield)

mc::field::ptr mc::cfield::getPtr(const rapidjson::Value& json)
{
	assert(json.IsObject());
	assert(json.HasMember("value"));
	assert(json["value"].IsNumber());
	assert(json.HasMember("type"));
	assert(json["type"].IsString());
	stype::code s = stype::codes.at(json["type"].GetString());
	switch (s)
	{
	case mc::stype::char_:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<int>())));
		break;
	case mc::stype::int2:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<int>())));
		break;
	case mc::stype::uint2:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<unsigned int>())));
		break;
	case mc::stype::int4:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<int>())));
		break;
	case mc::stype::uint4:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<unsigned int>())));
		break;
	case mc::stype::int8:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<int64_t>())));
		break;
	case mc::stype::uint8:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<uint64_t>())));
		break;
	case mc::stype::int16:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<int64_t>())));
		break;
	case mc::stype::uint16:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<uint64_t>())));
		break;
	case mc::stype::float4:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<float>())));
		break;
	case mc::stype::float8:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<double>())));
		break;
	case mc::stype::float16:
		return field::ptr(new cfield(s, anyType::get(s, json["value"].Get<double>())));
		break;
	default:
		throw std::logic_error("wrong stype");
		break;
	}
}
