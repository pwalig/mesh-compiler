#include "reader.h"
#include <assimp/Importer.hpp>
#include <stdexcept>
#include <shared_mutex>

std::shared_mutex mutex_;

assimp::weightsData assimp::meshWeightsMap;
assimp::skeletonData assimp::meshSkeletonsMap;

void assimp::readFile(const std::string& pFile, std::function<void(const aiScene*)> process_scene, const unsigned int& pFlags)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(pFile, pFlags);

    // If the import failed, report it
    if (nullptr == scene) {
        throw std::runtime_error(importer.GetErrorString());
    }

    // Now we can access the file's contents.
    process_scene(scene);


    meshWeightsMap.clear();
    meshSkeletonsMap.clear();

    // We're done. Everything will be cleaned up by the importer destructor
}
