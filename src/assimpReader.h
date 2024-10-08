#pragma once
#include <string>
#include <functional>
#include <vector>
#include <limits>
#include <array>

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
        meshWeights(const aiMesh* mesh);
        std::vector<vertex> vertices;
    };

    class skeleton {
    public:
        class bone {
        public:
            bone();
            bone(const aiMatrix4x4 offset_matrix);
            aiMatrix4x4 offset_matrix;
            aiVector3D position;
            aiVector3D rotation;
            aiVector3D scale;
            void setData(const aiMatrix4x4 offset_matrix);
        };
        skeleton(const aiMesh* mesh);
        std::vector<bone> bones;
    };

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
        for (int i = 0; i < MAX; ++i) {
            if (weight > this->weights[i]) {
                for (int j = MAX - 1; j > i; --j) {
                    this->weights[j] = this->weights[j-1];
                    this->bone_ids[j] = this->bone_ids[j-1];
                }
                this->weights[i] = weight;
                this->bone_ids[i] = bone_id;
                return;
            }
        }
    }

    template<typename T, typename U, unsigned int MAX>
    inline void meshWeights<T, U, MAX>::vertex::setData(const T& bone_id, const U& weight, const unsigned int& index)
    {
        this->weights[index] = weight;
        this->bone_ids[index] = bone_id;
    }

    template<typename T, typename U, unsigned int MAX>
    inline meshWeights<T, U, MAX>::meshWeights(const aiMesh* mesh)
    {
        this->vertices.resize(mesh->mNumVertices);

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            unsigned int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (unsigned int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = mesh->mBones[boneIndex]->mWeights[weightIndex].mVertexId;
                U weight = mesh->mBones[boneIndex]->mWeights[weightIndex].mWeight;

                this->vertices[vertexId].setData(boneIndex, weight);
            }
        }
    }

    inline skeleton::bone::bone()
    {}

    inline skeleton::bone::bone(const aiMatrix4x4 offset_matrix)
    {
        this->setData(offset_matrix);
    }

    inline void skeleton::bone::setData(const aiMatrix4x4 offset_matrix)
    {
        this->offset_matrix = offset_matrix;
        this->offset_matrix.Decompose(this->scale, this->rotation, this->position);
    }

    inline skeleton::skeleton(const aiMesh* mesh)
    {
        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            bone b(mesh->mBones[boneIndex]->mOffsetMatrix);
            bones.push_back(b);
        }
    }
}