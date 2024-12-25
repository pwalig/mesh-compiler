#include "compilation-info.h"
#include "../assimpReader.h"

void mc::compilationInfo::compileFile(const std::string& filename) {
    for (fileUnit& fu : file_units) {

        std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
        const size_t p(base_filename.find_last_of('.'));
        fu.withChangedName("{file}", base_filename.substr(0, p), [&filename](fileUnit* fu) {
            // assimp::readFile(filename, std::bind(&mc::fileUnit::compile, fu, std::placeholders::_1));
            });
    }
}