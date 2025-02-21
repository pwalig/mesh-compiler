#include "boneNode.h"
#include "boneChildNode.h"

assimp::boneNode::boneNode(const aiSkeleton* skeleton_, unsigned long id_) :
    skeleton(skeleton_), id(id_), mc::Inode(mc::ctype::per_bone)
{
}

std::string assimp::boneNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::boneNode::getCount() const
{
    return skeleton->mNumBones;
}

mc::anyType::value assimp::boneNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::offset_matrix:
        mc::anyType::setValue(va, skeleton->mBones[id]->mOffsetMatrix[suffixes[0]][suffixes[1]], s);
        break;
    case mc::vtype::bone_parent:
        mc::anyType::setValue(va, skeleton->mBones[id]->mParent, s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::boneNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    switch (counting_type)
    {
    case mc::ctype::per_bone_child:
        return skeleton->mBones[id]->mNode->mNumChildren;
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::boneNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_bone_child:
        return mc::Inode::ptr(new boneChildNode(skeleton, (unsigned long)this->id, (unsigned long)id));
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

oop_ptr_define(mc::Inode, assimp::boneNode)
