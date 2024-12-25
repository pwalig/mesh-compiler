#include "faceNode.h"

assimp::faceNode::faceNode(const aiMesh* mesh_, unsigned long id_) :
    mesh(mesh_), id(id_), mc::Inode(mc::ctype::per_indice)
{
}

std::string assimp::faceNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::faceNode::getCount() const
{
    return mesh->mNumFaces;
}

mc::anyType::value assimp::faceNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::indice:
        mc::anyType::setValue(va, mesh->mFaces[id].mIndices[suffixes[0]], s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::faceNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::faceNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::faceNode)
