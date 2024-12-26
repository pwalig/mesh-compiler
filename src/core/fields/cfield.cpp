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
