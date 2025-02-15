#include "scalingKeyNode.h"

assimp::scalingKeyNode::scalingKeyNode(const aiNodeAnim* channel_, unsigned long id_) :
    channel(channel_), id(id_), mc::Inode(mc::ctype::per_scale_keyframe)
{
}

std::string assimp::scalingKeyNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::scalingKeyNode::getCount() const
{
    return channel->mNumScalingKeys;
}

mc::anyType::value assimp::scalingKeyNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::scale_key:
        mc::anyType::setValue(va, channel->mScalingKeys[id].mValue[suffixes[0]], s);
        break;
    case mc::vtype::scale_key_timestamp:
        mc::anyType::setValue(va, channel->mScalingKeys[id].mTime, s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::scalingKeyNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::scalingKeyNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::scalingKeyNode)
