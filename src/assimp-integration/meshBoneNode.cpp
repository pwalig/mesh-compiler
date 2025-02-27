#include "meshBoneNode.h"
#include "reader.h"
#include "meshBoneChildNode.h"

assimp::meshBoneNode::meshBoneNode(const aiMesh* mesh_, unsigned long id_) :
    mesh(mesh_), id(id_), mc::Inode(mc::ctype::per_mesh_bone)
{
}

std::string assimp::meshBoneNode::getName() const
{
    return mesh->mBones[id]->mName.C_Str();
}

size_t assimp::meshBoneNode::getCount() const
{
    return mesh->mNumBones;
}

mc::anyType::value assimp::meshBoneNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::mesh_bone_offset_matrix:
        mc::anyType::setValue(va, mesh->mBones[id]->mOffsetMatrix[suffixes[0]][suffixes[1]], s);
        break;
    case mc::vtype::mesh_bone_parent:
        mc::anyType::setValue(va, meshSkeletonsMap.get(mesh).bones[id].parent, s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::meshBoneNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    switch (counting_type)
    {
    case mc::ctype::per_mesh_bone_child:
        return meshSkeletonsMap.get(mesh).bones[id].children.size();
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

mc::Inode::ptr assimp::meshBoneNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    switch (counting_type)
    {
    case mc::ctype::per_mesh_bone_child:
        return mc::Inode::ptr(new meshBoneChildNode(mesh, (unsigned long)this->id, (unsigned long)id));
        break;
    default:
        throw std::logic_error("invalid counting type");
        break;
    }
}

oop_ptr_define(mc::Inode, assimp::meshBoneNode)
