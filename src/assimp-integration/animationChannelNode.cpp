#include "animationChannelNode.h"
#include "positionKeyNode.h"
#include "rotationKeyNode.h"
#include "scalingKeyNode.h"

assimp::animationChannelNode::animationChannelNode(const aiAnimation* animation_, const aiNodeAnim* channel_) :
    animation(animation_), channel(channel_), mc::Inode(mc::ctype::per_animation_channel)
{
}

std::string assimp::animationChannelNode::getName() const
{
    return channel->mNodeName.C_Str();
}

size_t assimp::animationChannelNode::getCount() const
{
    return animation->mNumChannels;
}

mc::anyType::value assimp::animationChannelNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    throw std::logic_error("no values");
}

size_t assimp::animationChannelNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    switch (counting_type)
    {
    case mc::ctype::per_position_keyframe:
        return channel->mNumPositionKeys;
        break;
    case mc::ctype::per_rotation_keyframe:
        return channel->mNumRotationKeys;
        break;
    case mc::ctype::per_scale_keyframe:
        return channel->mNumScalingKeys;
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::animationChannelNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_position_keyframe:
        return mc::Inode::ptr(new positionKeyNode(channel, (unsigned long)id));
        break;
    case mc::ctype::per_rotation_keyframe:
        return mc::Inode::ptr(new rotationKeyNode(channel, (unsigned long)id));
        break;
    case mc::ctype::per_scale_keyframe:
        return mc::Inode::ptr(new scalingKeyNode(channel, (unsigned long)id));
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

oop_ptr_define(mc::Inode, assimp::animationChannelNode)
