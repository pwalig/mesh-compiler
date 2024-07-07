#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>

namespace mesh_compiler
{
    struct compileConfig;

    struct compilationInfo {
        bool indices = true;
        bool debug_messages = false;
        std::string format_file = ".format";
        std::string output_file = "{file}_{mesh}.mesh";
        compileConfig* config = nullptr;

        ~compilationInfo();
    };

	void run(int argc, char** argv);
	int compile(const std::vector<std::string>& args);
	int compileFile(const std::string& filename, compilationInfo& ci);
	int compileScene(const aiScene* scene, compilationInfo& ci);
    int compileMesh(const aiMesh* m, compilationInfo& ci);
}