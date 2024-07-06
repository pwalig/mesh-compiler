#ifndef tests_included
#define tests_included

#include <vector>

#include <assimp/scene.h>


struct mesh {
    std::vector<unsigned int> indices;
    std::vector<float> verts;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> bitangents;
};

void printMesh(const mesh& m);

void readMeshFile(const std::string& filepath, mesh& me);

void readMesh(const aiScene* scene, mesh& me);

void mainTest();

#endif // tests_included