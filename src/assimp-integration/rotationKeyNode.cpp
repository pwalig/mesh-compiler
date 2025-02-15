#include "rotationKeyNode.h"

assimp::rotationKeyNode::rotationKeyNode(const aiNodeAnim* channel_, unsigned long id_) :
    channel(channel_), id(id_), mc::Inode(mc::ctype::per_indice)
{
}

std::string assimp::rotationKeyNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::rotationKeyNode::getCount() const
{
    return channel->mNumPositionKeys;
}

mc::anyType::value assimp::rotationKeyNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};
	ai_real val = 0;

    switch (v)
    {
    case mc::vtype::rotation_key:
        if (suffixes[0] == 0) val = channel->mRotationKeys[id].mValue.x;
        else if (suffixes[0] == 1) val = channel->mRotationKeys[id].mValue.y;
        else if (suffixes[0] == 2) val = channel->mRotationKeys[id].mValue.z;
        else if (suffixes[0] == 3) val = channel->mRotationKeys[id].mValue.w;
        mc::anyType::setValue(va, val, s);
        break;
    case mc::vtype::rotation_key_timestamp:
        mc::anyType::setValue(va, channel->mRotationKeys[id].mTime, s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::rotationKeyNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::rotationKeyNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::rotationKeyNode)
