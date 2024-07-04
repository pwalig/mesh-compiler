#include "meshFileReader.h"
#include <iostream>

void mesh_reader::querryBuffers(const std::string& filepath, unsigned short int& buffers)
{
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file) {
        std::cout << "Cannot open file!" << std::endl;
        return;
    }

    unsigned short int buffs = 0;
    file.read((char*)&buffs, sizeof(unsigned short int));

    file.close();
}

void mesh_reader::fillBuffer(std::ifstream& file, char* buffer)
{
    file.read(buffer, sizeof(int));
}

void mesh_reader::querryBufferSizes(const std::string& filepath, unsigned int& sizes ...)
{
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file) {
        std::cout << "Cannot open file!" << std::endl;
        return;
    }


    file.close();
}

void mesh_reader::fillBuffers(const std::string& filepath, char* buffers ...)
{
}

void mesh_reader::querryBuffers(std::ifstream& file, unsigned short int& buffers, std::vector<unsigned int>& sizes)
{
    file.read((char*)&buffers, sizeof(unsigned short int));
    std::cout << " buffs: " << buffers << std::endl;

    sizes.resize(buffers);
    file.read((char*)(sizes.data()), buffers * sizeof(unsigned int));
}

void mesh_reader::querryBufferSizes(std::ifstream& file, unsigned int& sizes ...)
{
}

void mesh_reader::fillBuffers(std::ifstream& file, char* buffers ...)
{
}
