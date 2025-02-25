#pragma once
#include "types/stype.h"
#include <ostream>

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
        inline value get(stype::code s, T val) {
            value v{};
            setValue<T>(v, val, s);
            return v;
        }

        // throws invalid argument exception if str is not a number
        inline value get(stype::code s, const std::string& str) {
            value v{};
            switch (s)
            {
            case mc::stype::char_:
                if (str.length() == 1) v.c = str[0];
                else v.c = (char)std::stoi(str);
                break;
            case mc::stype::int2:
                v.s = (short)std::stoi(str);
                break;
            case mc::stype::uint2:
                v.us = (unsigned short)std::stoul(str);
                break;
            case mc::stype::int4:
                v.i = std::stoi(str);
                break;
            case mc::stype::uint4:
                v.ui = (unsigned int)std::stoul(str);
                break;
            case mc::stype::int8:
                v.l = std::stol(str);
                break;
            case mc::stype::uint8:
                v.ul = std::stoul(str);
                break;
            case mc::stype::int16:
                v.ll = std::stoll(str);
                break;
            case mc::stype::uint16:
                v.ull = std::stoull(str);
                break;
            case mc::stype::float4:
                v.f = std::stof(str);
                break;
            case mc::stype::float8:
                v.d = std::stod(str);
                break;
            case mc::stype::float16:
                v.ld = std::stold(str);
                break;
            default:
                throw std::invalid_argument("no conversion available");
                break;
            }
            return v;
        }

        inline void output(value v, stype::code s, std::ostream& os) {
            switch (s)
            {
            case mc::stype::char_:
                os << v.c;
                break;
            case mc::stype::int2:
                os << v.s;
                break;
            case mc::stype::uint2:
                os << v.us;
                break;
            case mc::stype::int4:
                os << v.i;
                break;
            case mc::stype::uint4:
                os << v.ui;
                break;
            case mc::stype::int8:
                os << v.l;
                break;
            case mc::stype::uint8:
                os << v.ul;
                break;
            case mc::stype::int16:
                os << v.ll;
                break;
            case mc::stype::uint16:
                os << v.ull;
                break;
            case mc::stype::float4:
                os << v.f;
                break;
            case mc::stype::float8:
                os << v.d;
                break;
            case mc::stype::float16:
                os << v.ld;
                break;
            default:
                throw std::logic_error("not supported type");
                break;
            }
        }

        inline void outputBinary(value v, stype::code s, std::ostream& os) {
            switch (s)
            {
            case mc::stype::char_:
                os.write((char*)(&v.c), sizeof(char));
                break;
            case mc::stype::int2:
                os.write((char*)(&v.s), sizeof(short));
                break;
            case mc::stype::uint2:
                os.write((char*)(&v.us), sizeof(unsigned short));
                break;
            case mc::stype::int4:
                os.write((char*)(&v.i), sizeof(int));
                break;
            case mc::stype::uint4:
                os.write((char*)(&v.ui), sizeof(unsigned int));
                break;
            case mc::stype::int8:
                os.write((char*)(&v.l), sizeof(long));
                break;
            case mc::stype::uint8:
                os.write((char*)(&v.ul), sizeof(unsigned long));
                break;
            case mc::stype::int16:
                os.write((char*)(&v.ll), sizeof(long long));
                break;
            case mc::stype::uint16:
                os.write((char*)(&v.ull), sizeof(unsigned long long));
                break;
            case mc::stype::float4:
                os.write((char*)(&v.f), sizeof(float));
                break;
            case mc::stype::float8:
                os.write((char*)(&v.d), sizeof(double));
                break;
            case mc::stype::float16:
                os.write((char*)(&v.ld), sizeof(long double));
                break;
            default:
                throw std::logic_error("not supported type");
                break;
            }
        }

        template<typename T>
        inline T getValue(value v, stype::code s) {
            switch (s)
            {
            case mc::stype::char_:
                return (T)v.c;
                break;
            case mc::stype::int2:
                return (T)v.s;
                break;
            case mc::stype::uint2:
                return (T)v.us;
                break;
            case mc::stype::int4:
                return (T)v.i;
                break;
            case mc::stype::uint4:
                return (T)v.ui;
                break;
            case mc::stype::int8:
                return (T)v.l;
                break;
            case mc::stype::uint8:
                return (T)v.ul;
                break;
            case mc::stype::int16:
                return (T)v.ll;
                break;
            case mc::stype::uint16:
                return (T)v.ull;
                break;
            case mc::stype::float4:
                return (T)v.f;
                break;
            case mc::stype::float8:
                return (T)v.d;
                break;
            case mc::stype::float16:
                return (T)v.ld;
                break;
            default:
                throw std::logic_error("not supported type");
                break;
            }
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