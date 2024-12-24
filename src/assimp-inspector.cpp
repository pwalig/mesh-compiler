#include "assimp-inspector.h"
#include <iostream>
#include <assimpReader.h>

#define MAX_BONE_INFLUENCE 4

void assimp::printMesh(const aiMesh* m, bool detailed)
{
    std::cout << "\tname: " << m->mName.C_Str() << "\n";
    std::cout << "\tvertices: " << m->mNumVertices;
    for (int j = 0; j < m->mNumVertices && detailed; ++j) {
        std::cout << " | (" << m->mVertices[j].x << " ; ";
        std::cout << m->mVertices[j].y << " ; ";
        std::cout << m->mVertices[j].z << ")";
    }
    std::cout << "\n";

    if (m->HasBones()) {
        assimp::meshWeights<int, float, MAX_BONE_INFLUENCE> mw(m);
        std::cout << "\tweights: [id:weight]\n";
        for (int i = 0; i < m->mNumVertices && detailed; ++i) {
            std::cout << "\t\t";
            for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
                std::cout << "[ " << mw.vertices[i].bone_ids[j] << " : ";
                std::cout << mw.vertices[i].weights[j] << " ]";
                if (j < m->mNumVertices) std::cout << " | ";
            }
            std::cout << "\n";
        }
        assimp::skeleton skeleton(m);
        std::cout << "\tbones: " << skeleton.bones.size() << "\n";
        if (detailed) {
            for (const assimp::skeleton::bone& b : skeleton.bones) {
                std::cout << "\t\tname: " << b.name << "\n";
                std::cout << "\t\tposition: (" << b.position.x << " ; "
                    << b.position.y << " ; " << b.position.z << ")\n";
                std::cout << "\t\trotations: (" << b.rotation.x << " ; "
                    << b.rotation.y << " ; " << b.rotation.z << ")\n";
                std::cout << "\t\tscale: (" << b.scale.x << " ; "
                    << b.scale.y << " ; " << b.scale.z << ")\n";
            }
        }
    }
}

void assimp::printMeshes(const aiScene* scene, bool detailed)
{
    std::cout << "meshes: " << scene->mNumMeshes << std::endl;
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        printMesh(scene->mMeshes[i], detailed);
    }
}

void assimp::printAnimation(const aiAnimation* a, bool detailed)
{
    std::cout << "\tname: " << a->mName.C_Str() << "\n ";
    std::cout << "\tduration: " << a->mDuration << "\n";
    std::cout << "\tticks per second: " << a->mTicksPerSecond << "\n";
    std::cout << "\tchannels: " << a->mNumChannels << "\n";
    for (int j = 0; j < a->mNumChannels; ++j) {
        aiNodeAnim* c = a->mChannels[j];
        std::cout << "\t\tname: " << c->mNodeName.C_Str() << "\n";
        std::cout << "\t\tposition keys: " << c->mNumPositionKeys << "\n";
        for (int k = 0; k < c->mNumPositionKeys && detailed; ++k) {
            std::cout << "\t\t\t" << c->mPositionKeys[k].mTime;
            std::cout << " : (" << c->mPositionKeys[k].mValue.x << " ; "
                << c->mPositionKeys[k].mValue.y << " ; "
                << c->mPositionKeys[k].mValue.z << ")\n";
        }
        std::cout << "\t\trotation keys: " << c->mNumRotationKeys << "\n";
        for (int k = 0; k < c->mNumRotationKeys && detailed; ++k) {
            std::cout << "\t\t\t" << c->mRotationKeys[k].mTime;
            std::cout << " : (" << c->mRotationKeys[k].mValue.x << " ; "
                << c->mRotationKeys[k].mValue.y << " ; "
                << c->mRotationKeys[k].mValue.z << " ; "
                << c->mRotationKeys[k].mValue.w << ")\n";
        }
        std::cout << "\t\tscaling keys: " << c->mNumScalingKeys << "\n";
        for (int k = 0; k < c->mNumScalingKeys && detailed; ++k) {
            std::cout << "\t\t\t" << c->mScalingKeys[k].mTime;
            std::cout << " : (" << c->mScalingKeys[k].mValue.x << " ; "
                << c->mScalingKeys[k].mValue.y << " ; "
                << c->mScalingKeys[k].mValue.z << ")\n";
        }
    }
}

void assimp::printAnimations(const aiScene* scene, bool detailed)
{
    std::cout << "animations: " << scene->mNumAnimations << std::endl;
    for (int i = 0; i < scene->mNumAnimations; ++i) {
        printAnimation(scene->mAnimations[i], detailed);
    }
}

void assimp::printSkeleton(const aiSkeleton* s, bool detailed)
{
    std::cout << "\tname: " << s->mName.C_Str() << "\n";
    std::cout << "\tbones: " << s->mNumBones << "\n";
    for (int i = 0; i < s->mNumBones && detailed; ++i) {
        std::cout << s->mBones[i]->mParent << "\n";
    }
}

void assimp::printSkeletons(const aiScene* scene, bool detailed)
{
    std::cout << "skeletons: " << scene->mNumSkeletons << std::endl;
    for (int i = 0; i < scene->mNumSkeletons; ++i) {
        printSkeleton(scene->mSkeletons[i], detailed);
    }
}

void assimp::printScene(const aiScene* scene, bool detailed)
{
    std::cout << "scene name: " << scene->mName.C_Str() << std::endl;
    printMeshes(scene, detailed);
    printSkeletons(scene, detailed);
    printAnimations(scene, detailed);
}
