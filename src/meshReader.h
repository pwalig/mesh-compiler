#pragma once
#include <fstream>
#include <vector>
#define allocation_limit 10485760 // 10 MB

namespace mesh_reader {
	template <typename T, typename U>
	void readBuffer(std::ifstream& file, std::vector<T>& buffer); // assumes count of type T values at front as type U value

	template <typename T, typename U>
	void readBuffer(std::ifstream& file, std::vector<T>& buffer, const U& count);
}

template<typename T, typename U>
inline void mesh_reader::readBuffer(std::ifstream& file, std::vector<T>& buffer)
{
	U count;
	file.read((char*)&count, sizeof(U));
	readBuffer<T, U>(file, buffer, count);
}

template<typename T, typename U>
inline void mesh_reader::readBuffer(std::ifstream& file, std::vector<T>& buffer, const U& count)
{
#ifdef allocation_limit
	if (count * sizeof(T) > allocation_limit)
		throw std::runtime_error("allocation limit exceeded");
#endif // allocation_limit

	buffer.resize(count);
	file.read((char*)(buffer.data()), count * sizeof(T));
}

#ifdef allocation_limit
#undef allocation_limit
#endif // allocation_limit
