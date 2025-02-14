#include "meshNode.h"
#include "vertexNode.h"
#include "indiceNode.h"

assimp::meshNode::meshNode(const aiScene* scene_, const aiMesh* mesh_) :
    scene(scene_), mesh(mesh_), mc::Inode(mc::ctype::per_mesh)
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

#pragma warning( push )
#pragma warning( disable : 4715 )

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
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::meshNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_indice:
        return mc::Inode::ptr(new indiceNode(mesh, (unsigned long)id));
        break;
    case mc::ctype::per_vertex:
        return mc::Inode::ptr(new vertexNode(mesh, (unsigned long)id));
        break;
    case mc::ctype::per_mesh_bone:
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

#pragma warning ( pop )

oop_ptr_define(mc::Inode, assimp::meshNode)
