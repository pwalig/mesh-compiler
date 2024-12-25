#include "sceneNode.h"
#include "meshNode.h"
#include "animationNode.h"

assimp::sceneNode::sceneNode(const aiScene* scene_) :
    scene(scene_), mc::Inode(mc::ctype::per_scene)
{
}

std::string assimp::sceneNode::getName() const
{
    return scene->mName.C_Str();
}

size_t assimp::sceneNode::getCount() const
{
    return 1;
}

mc::anyType::value assimp::sceneNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    throw std::logic_error("no values");
}

size_t assimp::sceneNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    switch (counting_type)
    {
    case mc::ctype::per_mesh:
        return scene->mNumMeshes;
        break;
    case mc::ctype::per_skeleton:
        return scene->mNumSkeletons;
        break;
    case mc::ctype::per_animation:
        return scene->mNumAnimations;
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::sceneNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_mesh:
        return mc::Inode::ptr(new meshNode(scene, scene->mMeshes[id]));
        break;
    case mc::ctype::per_skeleton:
        break;
    case mc::ctype::per_animation:
        return mc::Inode::ptr(new animationNode(scene, scene->mAnimations[id]));
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

oop_ptr_define(mc::Inode, assimp::sceneNode)
