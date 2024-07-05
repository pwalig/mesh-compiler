#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <functional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "meshCompiler.h"
#include "meshReader.h"

struct mesh {
    std::vector<unsigned int> indices;
    std::vector<float> verts;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> bitangents;
};

void printMesh(const mesh& m) {
    std::cout << "Mesh info\nindices: ";
    for (const unsigned int& indi : m.indices) std::cout << indi << " ";
    std::cout << "\n verts: ";
    for (const float& vert : m.verts) std::cout << vert << " ";
    std::cout << "\n normals: ";
    for (const float& vert : m.normals) std::cout << vert << " ";
    std::cout << "\n uvs: ";
    for (const float& vert : m.uvs) std::cout << vert << " ";
    std::cout << "\n tangents: ";
    for (const float& vert : m.tangents) std::cout << vert << " ";
    std::cout << "\n bitangents: ";
    for (const float& vert : m.bitangents) std::cout << vert << " ";
    std::cout << std::endl;
}

void ReadMeshFile(const std::string& filepath, mesh& me) {
    std::ifstream fin(filepath, std::ios::out | std::ios::binary);
    if (!fin) {
        std::cout << "Cannot open file!" << std::endl;
        exit(1);
    }
    
    mesh_reader::readBuffer<unsigned int, unsigned int>(fin, me.indices);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.verts);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.normals);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.uvs);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.tangents);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.bitangents);

    fin.close();
}

void ReadMesh(const aiScene* scene, mesh& me) {
    if (scene->mNumMeshes > 0) {
        aiMesh* m = scene->mMeshes[0];
        std::cout << m->mName.C_Str() << std::endl;
        for (int i = 0; i < m->mNumVertices; ++i) {
            me.verts.push_back(m->mVertices[i].x);
            me.verts.push_back(m->mVertices[i].y);
            me.verts.push_back(m->mVertices[i].z);
            me.normals.push_back(m->mNormals[i].x);
            me.normals.push_back(m->mNormals[i].y);
            me.normals.push_back(m->mNormals[i].z);
            me.uvs.push_back(m->mTextureCoords[0][i].x);
            me.uvs.push_back(m->mTextureCoords[0][i].y);
            me.uvs.push_back(m->mTextureCoords[0][i].z);
            me.tangents.push_back(m->mTangents[i].x);
            me.tangents.push_back(m->mTangents[i].y);
            me.tangents.push_back(m->mTangents[i].z);
            me.bitangents.push_back(m->mBitangents[i].x);
            me.bitangents.push_back(m->mBitangents[i].y);
            me.bitangents.push_back(m->mBitangents[i].z);
        }
        for (int i = 0; i < m->mNumFaces; ++i) {
            for (int j = 0; j < m->mFaces[i].mNumIndices; ++j) {
                me.indices.push_back(m->mFaces[i].mIndices[j]);
            }
        }
    }
}

bool ReadFile(const std::string& pFile, std::function<void(const aiScene*)> process_scene) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(pFile,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    // If the import failed, report it
    if (nullptr == scene) {
        printf("%s\n", importer.GetErrorString());
        return false;
    }

    // Now we can access the file's contents.
    process_scene(scene);

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}


int main() {
    std::function<void(const aiScene* scene)> nothing = [](const aiScene* scene) {};
    mesh me;
    const auto start{ std::chrono::steady_clock::now() };
    ReadFile("test/test.obj", std::bind(ReadMesh, std::placeholders::_1, std::ref(me)));
    //ReadFile("Ghost2.glb", nothing);
    const auto end{ std::chrono::steady_clock::now() };
    const std::chrono::duration<double> elapsed_seconds{ end - start };

    std::cout << "Vert Count: " << me.verts.size() / 3 << '\n';
    std::cout << "Indices Count: " << me.indices.size() / 3 << '\n';
    printMesh(me);
    std::cout << "Assimp read time: " << elapsed_seconds.count() << '\n';

    mesh_compiler::compilationInfo ci;
    ci.name = "test/out";
    ci.format_file = "test/.format";
    ci.debug_messages = true;
    ReadFile("test/test.obj", std::bind(mesh_compiler::compile, std::placeholders::_1, ci));

    mesh me1;
    const auto start1{ std::chrono::steady_clock::now() };
    ReadMeshFile("test/out.mesh", me1);
    const auto end1{ std::chrono::steady_clock::now() };
    const std::chrono::duration<double> elapsed_seconds1{ end1 - start1 };

    std::cout << "Vert Count: " << me1.verts.size() / 3 << '\n';
    std::cout << "Indices Count: " << me1.indices.size() / 3 << '\n';
    printMesh(me1);
    std::cout << "Binary read time: " << elapsed_seconds1.count() << '\n';
    
    return 0;
}