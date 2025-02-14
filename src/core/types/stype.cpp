#include "stype.h"

const std::unordered_map<std::string, mc::stype::code> mc::stype::codes = {
    {"char", char_},
    {"short", int2},
    {"int", int4},
    {"long", int8},
    {"long_long", int16},
    {"int2", int2},
    {"int4", int4},
    {"int8", int8},
    {"int16", int16},
    {"unsigned_short", uint2},
    {"unsigned_int", uint4},
    {"unsigned_long", uint8},
    {"unsigned_long_long", uint16},
    {"unsigned_int2", uint2},
    {"unsigned_int4", uint4},
    {"unsigned_int8", uint8},
    {"unsigned_int16", uint16},
    {"ushort", uint2},
    {"uint", uint4},
    {"ulong", uint8},
    {"uint2", uint2},
    {"uint4", uint4},
    {"uint8", uint8},
    {"uint16", uint16},
    {"float", float4},
    {"float4", float4},
    {"double", float8},
    {"float8", float8},
    {"long_double", float16},
    {"float16", float16}
};

const std::unordered_map<mc::stype::code, std::string> mc::stype::names = {
    {null, "no-stype"},
    {uint4, "unit"},
    {char_, "char"},
    {uint2, "int2"},
    {int4, "int4"},
    {int8, "int8"},
    {int16, "int16"},
    {uint2, "uint2"},
    {uint4, "uint4"},
    {uint8, "uint8"},
    {uint16, "uint16"},
    {float4, "float4"},
    {float8, "float8"},
    {float16, "float16"}
};

const std::unordered_map<mc::stype::code, unsigned short> mc::stype::sizes = {
    {char_, (unsigned short)sizeof(char)},
    {uint2, (unsigned short)sizeof(short)},
    {int4, (unsigned short)sizeof(int)},
    {int8, (unsigned short)sizeof(long)},
    {int16, (unsigned short)sizeof(long long)},
    {uint2, (unsigned short)sizeof(unsigned short)},
    {uint4, (unsigned short)sizeof(unsigned int)},
    {uint8, (unsigned short)sizeof(unsigned long)},
    {uint16, (unsigned short)sizeof(unsigned long long)},
    {float4, (unsigned short)sizeof(float)},
    {float8, (unsigned short)sizeof(double)},
    {float16, (unsigned short)sizeof(long double)}
};
