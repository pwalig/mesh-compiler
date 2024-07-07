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

	void compile(int argc, char** argv);
	int compileFile(const std::string& filename, compilationInfo& ci);
	int compileScene(const aiScene* scene, compilationInfo& ci);
    int compileMesh(const aiMesh* m, compilationInfo& ci);
}