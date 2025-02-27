#pragma once
#include <stdexcept>
#include <cstdint>
#include <cassert>

#if defined(LITTLE_ENDIAN) || defined(BIG_ENDIAN)
#define COMPTIME_ENDIAN
#endif

namespace endian {
	enum class ess {
		little,
		big,
		network = big,
#if defined(LITTLE_ENDIAN)
		host = little
#elif defined(BIG_ENDIAN)
		host = big
#endif
	};

#ifdef COMPTIME_ENDIAN
	constexpr ess host();
#else
	void init();
	ess host();
#endif

	template<typename T, size_t sz>
	struct byte_swapper
	{
		inline T operator()(T val)
		{
			throw std::out_of_range("data size");
		}
	};

	template<typename T>
	struct byte_swapper<T, 1>
	{
		inline T operator()(T val)
		{
			return val;
		}
	};

	template<typename T>
	struct byte_swapper<T, 2>
	{
		inline T operator()(T val)
		{
			return ((((val) >> 8) & 0xff) | (((val) & 0xff) << 8));
		}
	};

	template<typename T>
	struct byte_swapper<T, 4>
	{
		inline T operator()(T val)
		{
			return ((((val) & 0xff000000) >> 24) |
					(((val) & 0x00ff0000) >>  8) |
					(((val) & 0x0000ff00) <<  8) |
					(((val) & 0x000000ff) << 24));
		}
	};

	template<>
	struct byte_swapper<float, 4>
	{
		inline float operator()(float val)
		{
			uint32_t mem =byte_swapper<uint32_t, sizeof(uint32_t)>()(*(uint32_t*)&val);
			return *(float*)&mem;
		}
	};

	template<typename T>
	struct byte_swapper<T, 8>
	{
		inline T operator()(T val)
		{
			return ((((val) & 0xff00000000000000ull) >> 56) |
					(((val) & 0x00ff000000000000ull) >> 40) |
					(((val) & 0x0000ff0000000000ull) >> 24) |
					(((val) & 0x000000ff00000000ull) >> 8 ) |
					(((val) & 0x00000000ff000000ull) << 8 ) |
					(((val) & 0x0000000000ff0000ull) << 24) |
					(((val) & 0x000000000000ff00ull) << 40) |
					(((val) & 0x00000000000000ffull) << 56));
		}
	};

	template<>
	struct byte_swapper<double, 8>
	{
		inline double operator()(double val)
		{
			uint64_t mem =byte_swapper<uint64_t, sizeof(uint64_t)>()(*(uint64_t*)&val);
			return *(double*)&mem;
		}
	};

	template<class T>
	inline T swap_bytes(T value)
	{
		return byte_swapper<T, sizeof(T)>()(value);
	}

	template<ess from, ess to, class T>
	struct converter
	{
		inline T operator()(T value)
		{
			return byte_swapper<T, sizeof(T)>()(value);
		}
	};

	// specialisations when attempting to converter to the same endianess
	template<class T> struct converter<ess::little, ess::little, T> { inline T operator()(T value) { return value; } };
	template<class T> struct converter<ess::big, ess::big, T> { inline T operator()(T value) { return value; } };

	template<ess from, ess to, class T>
	inline T convert(T value)
	{
		assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);

		return converter<from, to, T>()(value);
	}
}
