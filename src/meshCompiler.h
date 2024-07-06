#pragma once
#include <string>
#include <assimp/scene.h>
#include <memory>

namespace mesh_compiler
{
    struct compileConfig;

    struct compilationInfo {
        bool indices = true;
        bool debug_messages = false;
        std::string format_file = ".format";
        std::string output_file = "out.mesh";
        compileConfig* config = nullptr;

        ~compilationInfo();
    };

	void compileFile(const std::string& filename, compilationInfo& ci);
	void compileScene(const aiScene* scene, compilationInfo& ci);
    void compileMesh(const aiMesh* m, compilationInfo& ci);
}