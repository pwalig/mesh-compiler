#pragma once
#include <vector>
#include <assimp/scene.h>
#include <fstream>
#include <functional>
#include "field.h"
#include "buffer.h"


namespace mc {
	class unit {
	public:
		std::vector<field*> preable;
		std::vector<buffer> buffers;
		ctype::code c;

		void outputScene(const std::ofstream& file, const aiScene* scene);
	};

	class fileUnit : public unit {
	public:
		std::string output_file;

		void compile(const aiScene* scene);

		void withChangedName(
			const std::string& pattern,
			const std::string& newName,
			const std::function<void(fileUnit*)>& func
		);
	};
}