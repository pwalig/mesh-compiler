#include "boneChildNode.h"

assimp::boneChildNode::boneChildNode(const aiSkeleton* skeleton_, unsigned long boneId, unsigned long childId) :
    skel(skeleton_), parentId(boneId), id(childId), mc::Inode(mc::ctype::per_bone_child)
{
}

std::string assimp::boneChildNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::boneChildNode::getCount() const
{
    return skel->mBones[parentId]->mNode->mNumChildren;
}

mc::anyType::value assimp::boneChildNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::bone_child:
        throw std::runtime_error("child of non mesh bone is not supported yet");
        //mc::anyType::setValue(va, skel->mBones[parentId]->mNode., s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::boneChildNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::boneChildNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::boneChildNode)
