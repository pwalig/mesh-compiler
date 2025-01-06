#include "compilation-info.h"
#include "../assimp-integration/reader.h"
#include "../assimp-integration/sceneNode.h"
#include <iostream>
#include <rapidjson/istreamwrapper.h>
#include "exceptions/jsonException.h"
#include "../jsonTools.h"

std::unordered_map<std::string, mc::unit> mc::compilationInfo::units;

mc::compilationInfo::compilationInfo(const std::string& filename)
{
    std::ifstream file(filename);
    size_t pos = filename.find_last_of('.');
    std::string extension = filename.substr(pos + 1, filename.size() - pos);
    if (extension == "json") {
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document document;
        document.ParseStream(isw);
        if (!document.IsObject()) throw jsonException("root of json file was not an object");

        const rapidjson::Value& junits = document["units"];
        if (!junits.IsArray()) throw jsonException("units was not an array");
        file_units.reserve(junits.Size());

        for (rapidjson::SizeType i = 0; i < junits.Size(); i++) {
            const rapidjson::Value& junit = junits[i];
            std::string ofilek = jsonTools::getAnyMember(junit, {"output_file", "outputFile", "output-file"});
            if (ofilek != "") {
                if(!junit[ofilek.c_str()].IsString()) throw jsonException(ofilek + " was not a string");
                file_units.push_back(mc::fileUnit(junit));
            }
            else if (junit.HasMember("name")) {
                if (!junit["name"].IsString()) throw jsonException("name was not a string");
                units.insert({ junit["name"].GetString(), mc::unit(junit) });
            }
            else throw jsonException("unit had neither \"name\" nor \"output_file\" member");
        }
    }
}

void mc::compilationInfo::compileFile(const std::string& filename) {
    // std::cout << "compiling file: " << filename << "\n";

    for (fileUnit& fu : file_units) {
        // std::cout << "file unit: " << fu.output_file << "\n";

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