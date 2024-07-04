#pragma once
#include "string"
#include <vector>

namespace mesh_compiler {
	std::vector<char> compileFormat(const std::string& file_name); // returns the post compilation byte code of the format file
	void compileFormat(const std::string& in_file, const std::string& out_file); // saves compiled code to out_file
}