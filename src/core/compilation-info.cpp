#include "compilation-info.h"
#include "../assimp-integration/reader.h"
#include "../assimp-integration/sceneNode.h"
#include <iostream>
#include <rapidjson/istreamwrapper.h>
#include "exceptions/jsonException.h"
#include "../jsonTools.h"
#include "exceptions/formatException.h"
#include <sstream>
#include "exceptions/compileException.h"
#include "types/ptype.h"

std::unordered_map<std::string, mc::unit> mc::compilationInfo::units;

mc::compilationInfo::compilationInfo(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("cannot open file: " + filename);
    }
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
    else if (extension == "format") {
		compilationContext context;
		std::string line;
		std::string word;
        while (std::getline(file, line)){
			context.linenum++;
			std::stringstream ss(line);
            while (ss >> word) {
                if (word == "begin") {
                    ss >> word;
                    if (word == "file") {
                        ss >> word; // get output filename

                        file_units.push_back(mc::fileUnit(file, word, context));

                        if (ss >> word) { // print mode
                            if (std::find(plainTextSpellings.begin(), plainTextSpellings.end(), word) != plainTextSpellings.end())
                                file_units.back().mode = mc::printMode::plainText;
							else if (std::find(littleEndianSpellings.begin(), littleEndianSpellings.end(), word) != littleEndianSpellings.end()) file_units.back().mode = mc::printMode::littleEndian;
							else if (std::find(bigEndianSpellings.begin(), bigEndianSpellings.end(), word) != bigEndianSpellings.end()) file_units.back().mode = mc::printMode::bigEndian;
                            else if (word == binarySpelling) file_units.back().mode = printMode::binary;
							else throw formatException("unknown token: " + word, context);
                        }
                    }
                    else {
						// check for unit name with keyword collision
						if (vtype::codes.find(word) != vtype::codes.end() ||
							ptype::codes.find(word) != ptype::codes.end())
							throw formatException("unit name: " + word + " collides with the keyword", context);
                        else
                            units.insert({ word, mc::unit(file, context) });
                    }
                }
                else throw formatException("unknown token: " + word, context);
            }
        }
    }
}

void mc::compilationInfo::compileFile(const std::string& filename, bool debug) {
    if (debug) std::cout << "compiling file: " << filename << "\n";

    assimp::readFile(filename, [this, filename, debug](const aiScene* scene) {
        for (fileUnit& fu : file_units) {
            if (debug) std::cout << "file unit: " << fu.output_file << "\n";

            std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
            const size_t p(base_filename.find_last_of('.'));

            std::string orig_name = fu.output_file;
            fu.changeName("{file}", base_filename.substr(0, p));

			fu.compile(Inode::ptr(new assimp::sceneNode(scene)), debug);

            fu.output_file = orig_name;
        }
        });
}
