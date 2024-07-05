#pragma once
#include <fstream>
#include <vector>

namespace mesh_reader {
	template <typename T, typename U>
	void readBuffer(std::ifstream& file, std::vector<T>& buffer); // assumes unsigned int at front

	template <typename T, typename U>
	void readBuffer(std::ifstream& file, std::vector<T>& buffer, const U& size);
}

template<typename T, typename U>
void mesh_reader::readBuffer(std::ifstream& file, std::vector<T>& buffer)
{
	U size;
	file.read((char*)&size, sizeof(U));
	readBuffer<T, U>(file, buffer, size);
}

template<typename T, typename U>
void mesh_reader::readBuffer(std::ifstream& file, std::vector<T>& buffer, const U& size)
{
	buffer.resize(size);
	file.read((char*)(buffer.data()), size * sizeof(T));
}