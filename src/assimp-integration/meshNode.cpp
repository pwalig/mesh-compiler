#include "meshNode.h"
#include "vertexNode.h"

assimp::meshNode::meshNode(const aiScene* scene_, const aiMesh* mesh_) : scene(scene_), mesh(mesh_)
{
}

std::string assimp::meshNode::getName() const
{
    return mesh->mName.C_Str();
}

size_t assimp::meshNode::getCount() const
{
    return scene->mNumMeshes;
}

mc::anyType::value assimp::meshNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    throw std::logic_error("no values");
}

size_t assimp::meshNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    switch (counting_type)
    {
    case mc::ctype::per_indice:
        return mesh->mNumFaces;
        break;
    case mc::ctype::per_vertex:
        return mesh->mNumVertices;
        break;
    case mc::ctype::per_mesh_bone:
        return mesh->mNumBones;
        break;
    default:
        break;
    }
}

mc::Inode::ptr assimp::meshNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_indice:
        break;
    case mc::ctype::per_vertex:
        return mc::Inode::ptr(new vertexNode(mesh, id));
        break;
    case mc::ctype::per_mesh_bone:
        break;
    default:
        break;
    }
}

oop_ptr_define(mc::Inode, assimp::meshNode)
