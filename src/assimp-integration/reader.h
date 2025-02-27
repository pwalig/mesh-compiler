#pragma once
#include <string>
#include <functional>
#include <vector>
#include <limits>
#include <array>
#include <unordered_map>
#include <shared_mutex>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <cassert>

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
            aiMatrix4x4 offset_matrix;
            aiVector3D position;
            aiVector3D rotation;
            aiVector3D scale;
            unsigned int parent;
            std::vector<unsigned int> children;

            bone() = default;
            bone(const aiBone* b);

            const std::string name;
            void setData(const aiMatrix4x4 offset_matrix);
        };
        skeleton(const aiMesh* mesh);
        std::vector<bone> bones;
    };

    template<typename T>
    class sharedMeshData {
    private:
        std::unordered_map<const aiMesh*, T> data;
        std::shared_mutex mutex;
        using reader_lock = std::shared_lock<std::shared_mutex>;
        using writer_lock = std::unique_lock<std::shared_mutex>;
    public:
        T get(const aiMesh* mesh); // thread safe
        void clear(); // not thread safe
    };

    using weightsData = sharedMeshData<meshWeights<unsigned int, ai_real, 4U>>;
    using skeletonData = sharedMeshData<skeleton>;

    extern weightsData meshWeightsMap;
    extern skeletonData meshSkeletonsMap;

	void readFile(const std::string& pFile, std::function<void(const aiScene*)> process_scene, const unsigned int& pFlags =
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_PopulateArmatureData);

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

    inline skeleton::bone::bone(const aiBone* b) : name(b->mName.C_Str())
    {
        this->setData(b->mOffsetMatrix);
    }

    inline void skeleton::bone::setData(const aiMatrix4x4 offset_matrix)
    {
        this->offset_matrix = offset_matrix;
        this->offset_matrix.Decompose(this->scale, this->rotation, this->position);
    }

    inline skeleton::skeleton(const aiMesh* mesh)
    {
        std::map<aiNode*, unsigned int> boneIndexMap;
        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            aiBone* aib = mesh->mBones[boneIndex];
            boneIndexMap.insert({ aib->mNode, boneIndex });
            bone b(aib);
            bones.push_back(b);
        }

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            aiNode* ain = mesh->mBones[boneIndex]->mNode;
            bone& b = bones[boneIndex];

            // parent
            {
                auto it = boneIndexMap.find(ain->mParent);
                if (it != boneIndexMap.end()) b.parent = it->second;
                else b.parent = -1;
            }

            // children
            for (unsigned int i = 0; i < ain->mNumChildren; ++i) {
                auto it = boneIndexMap.find(ain->mChildren[i]);
                assert(it != boneIndexMap.end());
                b.children.push_back(it->second);
            }
        }
    }
    template<typename T>
    inline T sharedMeshData<T>::get(const aiMesh* mesh)
    {
        {
            reader_lock rl(mutex);
            auto it = data.find(mesh);
            if (it != data.end()) return it->second;
        }
        {
            writer_lock wl(mutex);
            if (data.find(mesh) == data.end()) { // check if some other thread managed to insert the data
				data.insert(std::pair<const aiMesh*, T>(mesh, T(mesh)));
            }
			return data.at(mesh);
        }
    }
    template<typename T>
    inline void sharedMeshData<T>::clear()
    {
        data.clear();
    }
}