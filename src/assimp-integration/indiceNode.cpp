#include "indiceNode.h"

assimp::indiceNode::indiceNode(const aiMesh* mesh_, unsigned long id_) :
    mesh(mesh_), id(id_), mc::Inode(mc::ctype::per_indice)
{
}

std::string assimp::indiceNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::indiceNode::getCount() const
{
    return mesh->mNumFaces;
}

mc::anyType::value assimp::indiceNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
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

size_t assimp::indiceNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::indiceNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::indiceNode)
