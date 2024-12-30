#pragma once
#include "types/stype.h"

namespace mc {
    namespace anyType {
        union value {
            char c;

            short s;
            int i;
            long l;
            long long ll;

            unsigned short us;
            unsigned int ui;
            unsigned long ul;
            unsigned long long ull;

            float f;
            double d;
            long double ld;
        };

        template<typename T>
        inline void setValue(value& val, T newValue, stype::code s) {
            switch (s)
            {
            case mc::stype::char_:
                val.c = (char)newValue;
                break;
            case mc::stype::int2:
                val.s = (short)newValue;
                break;
            case mc::stype::uint2:
                val.us = (unsigned short)newValue;
                break;
            case mc::stype::int4:
                val.i = (int)newValue;
                break;
            case mc::stype::uint4:
                val.ui = (unsigned int)newValue;
                break;
            case mc::stype::int8:
                val.l = (long)newValue;
                break;
            case mc::stype::uint8:
                val.ul = (unsigned long)newValue;
                break;
            case mc::stype::int16:
                val.ll = (long long)newValue;
                break;
            case mc::stype::uint16:
                val.ull = (unsigned long long)newValue;
                break;
            case mc::stype::float4:
                val.f = (float)newValue;
                break;
            case mc::stype::float8:
                val.d = (double)newValue;
                break;
            case mc::stype::float16:
                val.ld = (long double)newValue;
                break;
            default:
                break;
            }
        }

        template<typename T>
        inline value getValue(stype::code s, T val) {
            value v{};
            setValue<T>(v, val, s);
            return v;
        }

        template<typename T>
        inline T getValue(value v) {
            throw std::logic_error("not supported type");
        }
        template<>
        inline char getValue(value v) { return v.c; }
        template<>
        inline short getValue(value v) { return v.s; }
        template<>
        inline unsigned short getValue(value v) { return v.us; }
        template<>
        inline int getValue(value v) { return v.i; }
        template<>
        inline unsigned int getValue(value v) { return v.ui; }
        template<>
        inline long getValue(value v) { return v.l; }
        template<>
        inline unsigned long getValue(value v) { return v.ul; }
        template<>
        inline long long getValue(value v) { return v.ll; }
        template<>
        inline unsigned long long getValue(value v) { return v.ull; }
        template<>
        inline float getValue(value v) { return v.f; }
        template<>
        inline double getValue(value v) { return v.d; }
        template<>
        inline long double getValue(value v) { return v.ld; }

        template<typename T>
        inline T& valueRef(value v) {
            throw std::logic_error("not supported type");
        }
        template<>
        inline char& valueRef(value v) { return v.c; }
        template<>
        inline short& valueRef(value v) { return v.s; }
        template<>
        inline unsigned short& valueRef(value v) { return v.us; }
        template<>
        inline int& valueRef(value v) { return v.i; }
        template<>
        inline unsigned int& valueRef(value v) { return v.ui; }
        template<>
        inline long& valueRef(value v) { return v.l; }
        template<>
        inline unsigned long& valueRef(value v) { return v.ul; }
        template<>
        inline long long& valueRef(value v) { return v.ll; }
        template<>
        inline unsigned long long& valueRef(value v) { return v.ull; }
        template<>
        inline float& valueRef(value v) { return v.f; }
        template<>
        inline double& valueRef(value v) { return v.d; }
        template<>
        inline long double& valueRef(value v) { return v.ld; }
    }
}