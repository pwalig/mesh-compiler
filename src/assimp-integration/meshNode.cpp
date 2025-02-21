#include "meshNode.h"
#include "vertexNode.h"
#include "faceNode.h"
#include "meshBoneNode.h"
#include "reader.h"

assimp::meshNode::meshNode(const aiScene* scene_, const aiMesh* mesh_) :
    scene(scene_), mesh(mesh_), mc::Inode(mc::ctype::per_mesh)
{
    if (meshWeightsMap.find(mesh_) == meshWeightsMap.end())
        meshWeightsMap.insert(std::pair<const aiMesh*, meshWeights<unsigned int, ai_real, 4U>>(mesh_, meshWeights<unsigned int, ai_real, 4U>(mesh_)));
    if (meshSkeletonsMap.find(mesh_) == meshSkeletonsMap.end())
        meshSkeletonsMap.insert(std::pair<const aiMesh*, skeleton>(mesh_, skeleton(mesh_)));
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
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::meshNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_indice:
        return mc::Inode::ptr(new faceNode(mesh, (unsigned long)id));
        break;
    case mc::ctype::per_vertex:
        return mc::Inode::ptr(new vertexNode(mesh, (unsigned long)id));
        break;
    case mc::ctype::per_mesh_bone:
        return mc::Inode::ptr(new meshBoneNode(mesh, (unsigned long)id));
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

oop_ptr_define(mc::Inode, assimp::meshNode)
