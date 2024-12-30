#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace mc {
    namespace stype {
        enum code : char {
            null,
            unit,
            char_,
            int2, uint2,
            int4, uint4,
            int8, uint8,
            int16, uint16,
            float4, float8, float16
        };

        extern const std::unordered_map<std::string, code> codes;
        extern const std::unordered_map<code, std::string> names;
        extern const std::unordered_map<code, unsigned short> sizes;

        template<typename T>
        inline code getCode()
        {
            throw std::logic_error("unsupported type");
        }
        template<>
        inline code getCode<char>() { return char_; }
        template<>
        inline code getCode<short>() { return int2; }
        template<>
        inline code getCode<unsigned short>() { return uint2; }
        template<>
        inline code getCode<int>() { return int4; }
        template<>
        inline code getCode<unsigned int>() { return uint4; }
        template<>
        inline code getCode<long>() { return int8; }
        template<>
        inline code getCode<unsigned long>() { return uint8; }
        template<>
        inline code getCode<long long>() { return int16; }
        template<>
        inline code getCode<unsigned long long>() { return uint16; }
        template<>
        inline code getCode<float>() { return float4; }
        template<>
        inline code getCode<double>() { return float8; }
        template<>
        inline code getCode<long double>() { return float16; }
    }
}