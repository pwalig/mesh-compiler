#pragma once
#include <string>
#include <functional>
#include <vector>
#include <limits>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace assimp {

// ========== DECLARATIONS ==========

    template <typename T, typename U, unsigned int MAX>
    class meshWeights {
    public:
        class vertex {
        public:
            std::array<T, MAX> bone_ids;
            std::array<U, MAX> weights;

            vertex();

            void setData(const T& bone_id, const U& weight);
        private:
            void setData(const T& bone_id, const U& weight, const unsigned int& index);
        };

        std::vector<vertex> vertices;
    };

    template <typename T, typename U, unsigned int MAX>
    inline meshWeights<T, U, MAX> getMeshWeights(const aiMesh* mesh);

	bool readFile(const std::string& pFile, std::function<void(const aiScene*)> process_scene, const unsigned int& pFlags =
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

// ========== DEFINITIONS ==========

    template<typename T, typename U, unsigned int MAX>
    inline meshWeights<T, U, MAX>::vertex::vertex()
    {
        for (int i = 0; i < MAX; ++i) {
            bone_ids[i] = -1;
            weights[i] = 0;
        }
    }

    template<typename T, typename U, unsigned int MAX>
    inline void meshWeights<T, U, MAX>::vertex::setData(const T& bone_id, const U& weight)
    {
        unsigned int min_id = MAX+1;
        U min_weight = std::numeric_limits<U>::max();

        for (int i = 0; i < MAX; ++i)
        {
            if (this->weights[i] < min_weight) {
                min_weight = this->weights[i];
                min_id = i;
            }
            if (this->bone_ids[i] < 0)
            {
                this->setData(bone_id, weight, i);
                return;
            }
        }
        if (weight > min_weight) {
            this->setData(bone_id, weight, min_id);
        }
    }

    template<typename T, typename U, unsigned int MAX>
    inline void meshWeights<T, U, MAX>::vertex::setData(const T& bone_id, const U& weight, const unsigned int& index)
    {
        this->weights[index] = weight;
        this->bone_ids[index] = bone_id;
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

                out.vertices[vertexId].setData(boneIndex, weight);
            }
        }

        return out;
    }
}