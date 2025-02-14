#include "animationNode.h"
#include "animationChannelNode.h"

assimp::animationNode::animationNode(const aiScene* scene_, const aiAnimation* animation_) :
    scene(scene_), animation(animation_), mc::Inode(mc::ctype::per_animation)
{
}

std::string assimp::animationNode::getName() const
{
    return animation->mName.C_Str();
}

size_t assimp::animationNode::getCount() const
{
    return scene->mNumAnimations;
}

mc::anyType::value assimp::animationNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::duration:
        mc::anyType::setValue(va, animation->mDuration, s);
        break;
    case mc::vtype::ticks_per_second:
        mc::anyType::setValue(va, animation->mTicksPerSecond, s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }

    return va;
}

#pragma warning( push )
#pragma warning( disable : 4715 )

size_t assimp::animationNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    switch (counting_type)
    {
    case mc::ctype::per_animation_channel:
        return animation->mNumChannels;
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::animationNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_indice:
        break;
    case mc::ctype::per_animation_channel:
        return mc::Inode::ptr(new animationChannelNode(animation, animation->mChannels[id]));
        break;
        throw std::logic_error("invalid counting type");
        break;
    }
}

#pragma warning ( pop )

oop_ptr_define(mc::Inode, assimp::animationNode)
