#pragma once

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
    }
}