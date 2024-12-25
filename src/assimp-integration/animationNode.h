#pragma once

#include "../core/Inode.h"
#include <assimp/scene.h>

namespace assimp {
	class animationNode : public mc::Inode {
	public:
		const aiScene* scene;
		const aiAnimation* animation;
		animationNode(const aiScene* scene_, const aiAnimation* animation_);

		std::string getName() const override;
		size_t getCount() const override;

		mc::anyType::value getValue(
			mc::vtype::code v, mc::stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const override;

		size_t getChildNodeCount(mc::ctype::code counting_type) const;
		mc::Inode::ptr getChildNodeOfType(mc::ctype::code counting_type, size_t id) const override;

		oop_ptr_child_declare(mc::Inode, animationNode);
	};
}