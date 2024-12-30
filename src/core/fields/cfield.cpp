#include "cfield.h"

mc::field::ptr mc::getCFieldPtr(stype::code s, anyType::value v)
{
	switch (s)
	{
	case mc::stype::char_:
		return field::ptr(new cfield<char>(anyType::getValue<char>(v)));
		break;
	case mc::stype::int2:
		return field::ptr(new cfield<short>(anyType::getValue<short>(v)));
		break;
	case mc::stype::uint2:
		return field::ptr(new cfield<unsigned short>(anyType::getValue<unsigned short>(v)));
		break;
	case mc::stype::int4:
		return field::ptr(new cfield<int>(anyType::getValue<int>(v)));
		break;
	case mc::stype::uint4:
		return field::ptr(new cfield<unsigned int>(anyType::getValue<unsigned int>(v)));
		break;
	case mc::stype::int8:
		return field::ptr(new cfield<long>(anyType::getValue<long>(v)));
		break;
	case mc::stype::uint8:
		return field::ptr(new cfield<unsigned long>(anyType::getValue<unsigned long>(v)));
		break;
	case mc::stype::int16:
		return field::ptr(new cfield<long long>(anyType::getValue<long long>(v)));
		break;
	case mc::stype::uint16:
		return field::ptr(new cfield<unsigned long long>(anyType::getValue<unsigned long long>(v)));
		break;
	case mc::stype::float4:
		return field::ptr(new cfield<float>(anyType::getValue<float>(v)));
		break;
	case mc::stype::float8:
		return field::ptr(new cfield<double>(anyType::getValue<double>(v)));
		break;
	case mc::stype::float16:
		return field::ptr(new cfield<long double>(anyType::getValue<long double>(v)));
		break;
	default:
		break;
	}
}

mc::field::ptr mc::getCFieldPtr(const rapidjson::Value& json)
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
		return field::ptr(new cfield<char>((char)json["value"].Get<int>()));
		break;
	case mc::stype::int2:
		return field::ptr(new cfield<short>((short)json["value"].Get<int>()));
		break;
	case mc::stype::uint2:
		return field::ptr(new cfield<unsigned short>((unsigned short)json["value"].Get<unsigned int>()));
		break;
	case mc::stype::int4:
		return field::ptr(new cfield<int>(json["value"].Get<int>()));
		break;
	case mc::stype::uint4:
		return field::ptr(new cfield<unsigned int>(json["value"].Get<unsigned int>()));
		break;
	case mc::stype::int8:
		return field::ptr(new cfield<long>((long)json["value"].Get<long long>()));
		break;
	case mc::stype::uint8:
		return field::ptr(new cfield<unsigned long>((unsigned long)json["value"].Get<unsigned long long>()));
		break;
	case mc::stype::int16:
		return field::ptr(new cfield<long long>(json["value"].Get<long long>()));
		break;
	case mc::stype::uint16:
		return field::ptr(new cfield<unsigned long long>(json["value"].Get<unsigned long long>()));
		break;
	case mc::stype::float4:
		return field::ptr(new cfield<float>(json["value"].Get<float>()));
		break;
	case mc::stype::float8:
		return field::ptr(new cfield<double>(json["value"].Get<double>()));
		break;
	case mc::stype::float16:
		return field::ptr(new cfield<long double>((long double)json["value"].Get<double>()));
		break;
	default:
		throw std::logic_error("wrong stype");
		break;
	}
}
