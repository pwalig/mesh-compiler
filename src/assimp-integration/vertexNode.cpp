#include "vertexNode.h"

assimp::vertexNode::vertexNode(const aiMesh* mesh_, unsigned long id_) :
    mesh(mesh_), id(id_), mc::Inode(mc::ctype::per_vertex)
{
}

std::string assimp::vertexNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::vertexNode::getCount() const
{
    return mesh->mNumVertices;
}

mc::anyType::value assimp::vertexNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::vertex:
        mc::anyType::setValue(va, mesh->mVertices[id][suffixes[0]], s);
        break;
    case mc::vtype::normal:
        mc::anyType::setValue(va, mesh->mNormals[id][suffixes[0]], s);
        break;
    case mc::vtype::tangent:
        mc::anyType::setValue(va, mesh->mTangents[id][suffixes[0]], s);
        break;
    case mc::vtype::bitangent:
        mc::anyType::setValue(va, mesh->mBitangents[id][suffixes[0]], s);
        break;
    case mc::vtype::uv:
        mc::anyType::setValue(va, mesh->mTextureCoords[id][suffixes[0]][suffixes[1]], s);
        break;
    case mc::vtype::vertex_color:
        mc::anyType::setValue(va, mesh->mColors[id][suffixes[0]][suffixes[1]], s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::vertexNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::vertexNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::vertexNode)
