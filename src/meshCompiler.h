#pragma once
#include <string>
#include <assimp/scene.h>

namespace mesh_compiler
{
    struct compilationInfo {
        bool indices = true;
        bool debug_messages = false;
        std::string format_file = ".format";
        std::string name = "out";
        std::string extension = ".mesh";
    };

	void compile(const aiScene* scene, const compilationInfo& ci);
    void compileMesh(const aiMesh* m, const compilationInfo& ci);

	void compile_old(const aiScene* scene, const compilationInfo& ci);
    void compileMesh_old(const aiMesh* m, const compilationInfo& ci);
}