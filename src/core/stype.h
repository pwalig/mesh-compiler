#pragma once
#include <unordered_map>
#include <string>

namespace mc {
    namespace stype {
        enum code {
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
    }
}