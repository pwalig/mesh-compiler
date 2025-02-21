#pragma once

#include "../core/Inode.h"
#include <assimp/scene.h>

namespace assimp {
	class boneChildNode : public mc::Inode {
	public:
		const aiSkeleton* skel;
		const long parentId, id;
		boneChildNode(const aiSkeleton* skeleton_, unsigned long boneId, unsigned long childId);

		std::string getName() const override;
		size_t getCount() const override;

		mc::anyType::value getValue(
			mc::vtype::code v, mc::stype::code s,
			const std::vector<unsigned short>& suffixes = std::vector<unsigned short>()
		) const override;

		size_t getChildNodeCount(mc::ctype::code counting_type) const;
		mc::Inode::ptr getChildNodeOfType(mc::ctype::code counting_type, size_t id) const override;

		oop_ptr_child_declare(mc::Inode);
	};
}