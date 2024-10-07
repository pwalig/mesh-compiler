#pragma once
#ifdef _DEBUG

#define MAX_BONE_INFLUENCE 4

#include <vector>
#include <array>

#include <assimp/scene.h>


struct mesh {
    std::vector<unsigned int> indices;
    std::vector<float> verts;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> bitangents;
    std::vector<int> bone_indexes;
    std::vector<float> bone_weights;
};

void printMesh(const mesh& m);

void readMeshFile(const std::string& filepath, mesh& me);

void readMesh(const aiScene* scene, mesh& me);

void mainTest();

#endif // _DEBUG