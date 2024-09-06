#pragma once
#include <string>
#include <functional>
#include <vector>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace assimp {
    template <typename T, typename U, unsigned int MAX>
    class vertex {
    public:
        T bone_ids[MAX];
        U weights[MAX];

        vertex();

        void setData(const T& bone_id, const U& weight);
    };

    template <typename T, typename U, unsigned int MAX>
    class meshWeights {
    public:
        std::vector<vertex<T, U, MAX>> vertices;
    };

    template <typename T, typename U, unsigned int MAX>
    inline meshWeights<T, U, MAX> getMeshWeights(const aiMesh* mesh);

	bool readFile(const std::string& pFile, std::function<void(const aiScene*)> process_scene, const unsigned int& pFlags =
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    template<typename T, typename U, unsigned int MAX>
    inline vertex<T, U, MAX>::vertex()
    {
        for (int i = 0; i < MAX; ++i) {
            bone_ids[i] = -1;
            weights[i] = 0;
        }
    }

    template<typename T, typename U, unsigned int MAX>
    inline void vertex<T, U, MAX>::setData(const T& bone_id, const U& weight)
    {
        for (int i = 0; i < MAX; ++i)
        {
            if (this->bone_ids[i] < 0)
            {
                this->weights[i] = weight;
                this->bone_ids[i] = bone_id;
                break;
            }
        }
    }

    template<typename T, typename U, unsigned int MAX>
    inline meshWeights<T, U, MAX> getMeshWeights(const aiMesh* mesh)
    {
        meshWeights<T, U, MAX> out;

        out.vertices.resize(mesh->mNumVertices);

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            unsigned int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (unsigned int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = mesh->mBones[boneIndex]->mWeights[weightIndex].mVertexId;
                U weight = mesh->mBones[boneIndex]->mWeights[weightIndex].mWeight;
                assert(vertexId <= out.vertices.size());

                out.vertices[vertexId].setData(boneIndex, weight);
            }
        }

        return out;
    }
}