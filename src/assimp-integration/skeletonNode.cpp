#include "skeletonNode.h"
#include "vertexNode.h"
#include "boneNode.h"
#include "reader.h"

assimp::skeletonNode::skeletonNode(const aiScene* scene_, const aiSkeleton* skeleton_) :
    scene(scene_), skeleton(skeleton_), mc::Inode(mc::ctype::per_skeleton)
{
}

std::string assimp::skeletonNode::getName() const
{
    return skeleton->mName.C_Str();
}

size_t assimp::skeletonNode::getCount() const
{
    return scene->mNumSkeletons;
}

mc::anyType::value assimp::skeletonNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    throw std::logic_error("no values");
}

size_t assimp::skeletonNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    switch (counting_type)
    {
    case mc::ctype::per_bone:
        return skeleton->mNumBones;
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::skeletonNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_bone:
        return mc::Inode::ptr(new boneNode(skeleton, (unsigned long)id));
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

oop_ptr_define(mc::Inode, assimp::skeletonNode)
