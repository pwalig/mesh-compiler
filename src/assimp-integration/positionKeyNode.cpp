#include "positionKeyNode.h"

assimp::positionKeyNode::positionKeyNode(const aiNodeAnim* channel_, unsigned long id_) :
    channel(channel_), id(id_), mc::Inode(mc::ctype::per_indice)
{
}

std::string assimp::positionKeyNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::positionKeyNode::getCount() const
{
    return channel->mNumPositionKeys;
}

mc::anyType::value assimp::positionKeyNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::position_key:
        mc::anyType::setValue(va, channel->mPositionKeys[id].mValue[suffixes[0]], s);
        break;
    case mc::vtype::position_key_timestamp:
        mc::anyType::setValue(va, channel->mPositionKeys[id].mTime, s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::positionKeyNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::positionKeyNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::positionKeyNode)
