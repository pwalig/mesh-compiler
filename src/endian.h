// code from: https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c

#pragma once
#include <stdexcept>
#include <cstdint>
#include <cassert>

enum class endianness
{
    little_endian,
    big_endian,
    network_endian = big_endian,
    
#if defined(LITTLE_ENDIAN)
	host_endian = little_endian
#elif defined(BIG_ENDIAN)
	host_endian = big_endian
#endif
};

endianness check_endianness();

endianness get_host_endianness();

void init_host_endianness();

namespace detail {

	template<typename T, size_t sz>
	struct swap_bytes
	{
		inline T operator()(T val)
		{
			throw std::out_of_range("data size");
		}
	};

	template<typename T>
	struct swap_bytes<T, 1>
	{
		inline T operator()(T val)
		{
			return val;
		}
	};

	template<typename T>
	struct swap_bytes<T, 2>
	{
		inline T operator()(T val)
		{
			return ((((val) >> 8) & 0xff) | (((val) & 0xff) << 8));
		}
	};

	template<typename T>
	struct swap_bytes<T, 4>
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
	struct swap_bytes<float, 4>
	{
		inline float operator()(float val)
		{
			uint32_t mem =swap_bytes<uint32_t, sizeof(uint32_t)>()(*(uint32_t*)&val);
			return *(float*)&mem;
		}
	};

	template<typename T>
	struct swap_bytes<T, 8>
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
	struct swap_bytes<double, 8>
	{
		inline double operator()(double val)
		{
			uint64_t mem =swap_bytes<uint64_t, sizeof(uint64_t)>()(*(uint64_t*)&val);
			return *(double*)&mem;
		}
	};

	template<endianness from, endianness to, class T>
	struct do_byte_swap
	{
		inline T operator()(T value)
		{
			return swap_bytes<T, sizeof(T)>()(value);
		}
	};
	// specialisations when attempting to swap to the same endianess
	template<class T> struct do_byte_swap<endianness::little_endian, endianness::little_endian, T> { inline T operator()(T value) { return value; } };
	template<class T> struct do_byte_swap<endianness::big_endian, endianness::big_endian, T> { inline T operator()(T value) { return value; } };

	template<class T>
	inline T byte_swap(T value)
	{
		return swap_bytes<T, sizeof(T)>()(value);
	}

} // namespace detail

template<endianness from, endianness to, class T>
inline T byte_swap(T value)
{
    // ensure the data is only 1, 2, 4 or 8 bytes
    assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);

    return detail::do_byte_swap<from, to, T>()(value);
}
