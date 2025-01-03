#include "compilation-info.h"
#include "../assimp-integration/reader.h"
#include "../assimp-integration/sceneNode.h"
#include <iostream>
#include <rapidjson/istreamwrapper.h>

mc::compilationInfo::compilationInfo(const std::string& filename)
{
    std::ifstream file(filename);
    size_t pos = filename.find_last_of('.');
    std::string extension = filename.substr(pos + 1, filename.size() - pos);
    if (extension == "json") {
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document document;
        document.ParseStream(isw);
        assert(document.IsObject());

        const rapidjson::Value& junits = document["units"];
        assert(junits.IsArray());
        file_units.reserve(junits.Size());

        for (rapidjson::SizeType i = 0; i < junits.Size(); i++) {
            const rapidjson::Value& junit = junits[i];
            assert(junit.HasMember("output_file") || junit.HasMember("name"));
            if (junit.HasMember("output_file")) {
                assert(junit["output_file"].IsString());
                file_units.push_back(mc::fileUnit(junit));
            }
            else {
                assert(junit["name"].IsString());
                units.insert({ junit["name"].GetString(), mc::unit(junit) });
                file_units.push_back(mc::fileUnit(junit));
            }
        }
    }
}

void mc::compilationInfo::compileFile(const std::string& filename) {
    std::cout << "compiling file: " << filename << "\n";

    for (fileUnit& fu : file_units) {
        std::cout << "file unit: " << fu.output_file << "\n";

        std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
        const size_t p(base_filename.find_last_of('.'));

        std::string orig_name = fu.output_file;
        fu.changeName("{file}", base_filename.substr(0, p));

        assimp::readFile(filename, [&fu](const aiScene* scene) {
            fu.compile(Inode::ptr(new assimp::sceneNode(scene)));
            });

        fu.output_file = orig_name;
    }
}