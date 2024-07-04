#pragma once
#include <string>
#include <fstream>
#include <vector>

namespace mesh_reader {
	void querryBuffers(const std::string& filepath, unsigned short int& buffers);
	void fillBuffer(std::ifstream& file, char* buffer);
	void querryBufferSizes(const std::string& filepath, unsigned int& sizes...);
	void fillBuffers(const std::string& filepath, char* buffers...);

	void querryBuffers(std::ifstream& file, unsigned short int& buffers, std::vector<unsigned int>& sizes);
	void querryBufferSizes(std::ifstream& file, unsigned int& sizes...);
	void fillBuffers(std::ifstream& file, char* buffers...);
}