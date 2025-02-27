#include "meshBoneChildNode.h"
#include "reader.h"

assimp::meshBoneChildNode::meshBoneChildNode(const aiMesh* mesh_, unsigned long boneId, unsigned long childId) :
    mesh(mesh_), parentId(boneId), id(childId), mc::Inode(mc::ctype::per_mesh_bone_child)
{
}

std::string assimp::meshBoneChildNode::getName() const
{
    return std::to_string(id);
}

size_t assimp::meshBoneChildNode::getCount() const
{
    return meshSkeletonsMap.get(mesh).bones[parentId].children.size();
}

mc::anyType::value assimp::meshBoneChildNode::getValue(mc::vtype::code v, mc::stype::code s, const std::vector<unsigned short>& suffixes) const
{
    mc::anyType::value va{};

    switch (v)
    {
    case mc::vtype::mesh_bone_child:
        mc::anyType::setValue(va, meshSkeletonsMap.get(mesh).bones[parentId].children[id], s);
        break;
    default:
        throw std::logic_error("invalid value type");
        break;
    }
    return va;
}

size_t assimp::meshBoneChildNode::getChildNodeCount(mc::ctype::code counting_type) const
{
    throw std::logic_error("no children");
}

mc::Inode::ptr assimp::meshBoneChildNode::getChildNodeOfType(mc::ctype::code counting_type, size_t id) const
{
    throw std::logic_error("no children");
}

oop_ptr_define(mc::Inode, assimp::meshBoneChildNode)
