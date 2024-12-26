#include "field.h"
#include "fields/cfield.h"
#include "fields/pfield.h"

mc::field::ptr mc::field::getPtr(const rapidjson::Value& json)
{
	assert(json.IsObject());
	assert(json.HasMember("value"));

	if (json["value"].IsNumber()) {
		assert(json.HasMember("type"));
		assert(json["type"].IsString());
		stype::code s = stype::codes.at(json["type"].GetString());
		switch (s)
		{
		case mc::stype::char_:
			return ptr(new cfield<char>((char)json.Get<int>()));
			break;
		case mc::stype::int2:
			return ptr(new cfield<short>((short)json.Get<int>()));
			break;
		case mc::stype::uint2:
			return ptr(new cfield<unsigned short>((unsigned short)json.Get<unsigned int>()));
			break;
		case mc::stype::int4:
			return ptr(new cfield<int>(json.Get<int>()));
			break;
		case mc::stype::uint4:
			return ptr(new cfield<unsigned int>(json.Get<unsigned int>()));
			break;
		case mc::stype::int8:
			return ptr(new cfield<long>((long)json.Get<long long>()));
			break;
		case mc::stype::uint8:
			return ptr(new cfield<unsigned long>((unsigned long)json.Get<unsigned long long>()));
			break;
		case mc::stype::int16:
			return ptr(new cfield<long long>(json.Get<long long>()));
			break;
		case mc::stype::uint16:
			return ptr(new cfield<unsigned long long>(json.Get<unsigned long long>()));
			break;
		case mc::stype::float4:
			return ptr(new cfield<float>(json.Get<float>()));
			break;
		case mc::stype::float8:
			return ptr(new cfield<double>(json.Get<double>()));
			break;
		case mc::stype::float16:
			return ptr(new cfield<long double>((long double)json.Get<double>()));
			break;
		default:
			throw std::logic_error("wrong stype");
			break;
		}
	}
	else {
		assert(json["value"].IsString());
		std::string value = json["value"].GetString();

		if (ptype::codes.find(value) != ptype::codes.end()) {
			return ptr(new pfield<unsigned int>(ptype::codes.at(value)));
		}
	}

	return ptr();
}
