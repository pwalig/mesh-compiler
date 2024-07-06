#include "assimpReader.h"
#include <assimp/Importer.hpp>

bool assimp::readFile(const std::string& pFile, std::function<void(const aiScene*)> process_scene, const unsigned int& pFlags)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(pFile, pFlags);

    // If the import failed, report it
    if (nullptr == scene) {
        printf("%s\n", importer.GetErrorString());
        return false;
    }

    // Now we can access the file's contents.
    process_scene(scene);

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}
