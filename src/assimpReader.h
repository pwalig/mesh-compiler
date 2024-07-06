#pragma once
#include <string>
#include <functional>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace assimp {
	bool readFile(const std::string& pFile, std::function<void(const aiScene*)> process_scene, const unsigned int& pFlags =
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);
}