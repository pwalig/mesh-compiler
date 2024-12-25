#pragma once

#include "../core/Inode.h"
#include <assimp/scene.h>

namespace assimp {
	class vertexNode : public mc::Inode {
	public:
		const aiMesh* mesh;
		const unsigned long id;
		vertexNode(const aiMesh* mesh_, unsigned long id_);

		std::string getName() const override;
		size_t getCount() const override;

		mc::anyType::value getValue(
			mc::vtype::code v, mc::stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const override;

		size_t getChildNodeCount(mc::ctype::code counting_type) const;
		mc::Inode::ptr getChildNodeOfType(mc::ctype::code counting_type, size_t id) const override;

		oop_ptr_child_declare(mc::Inode, vertexNode);
	};
}