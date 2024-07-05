#pragma once
#include <fstream>
#include <vector>

namespace mesh_reader {
	template <typename T, typename U>
	void readBuffer(std::ifstream& file, std::vector<T>& buffer); // assumes count of type T values at front as type U value

	template <typename T, typename U>
	void readBuffer(std::ifstream& file, std::vector<T>& buffer, const U& count);
}

template<typename T, typename U>
void mesh_reader::readBuffer(std::ifstream& file, std::vector<T>& buffer)
{
	U count;
	file.read((char*)&count, sizeof(U));
	readBuffer<T, U>(file, buffer, count);
}

template<typename T, typename U>
void mesh_reader::readBuffer(std::ifstream& file, std::vector<T>& buffer, const U& count)
{
	buffer.resize(count);
	file.read((char*)(buffer.data()), count * sizeof(T));
}