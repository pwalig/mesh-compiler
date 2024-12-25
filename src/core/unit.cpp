#include "unit.h"

void mc::unit::outputScene(const std::ofstream& file, const aiScene* scene)
{
}

void mc::fileUnit::compile(const aiScene* scene)
{
    withChangedName("{scene}", scene->mName.C_Str(), [scene](fileUnit* fu) {
        std::ofstream fout(fu->output_file, std::ios::out | std::ios::binary);
        if (!fout) {
            throw std::runtime_error("cannot open file: " + fu->output_file);
        }
        fu->outputScene(fout, scene);
        fout.close();
        });
}

void mc::fileUnit::withChangedName(
    const std::string& pattern, const std::string& newName,
    const std::function<void(fileUnit*)>& func
) {
    std::string orig_name = output_file;
    size_t found = output_file.find(pattern);
    if (found != std::string::npos) output_file.replace(found, pattern.length(), newName);

    func(this);

    output_file = orig_name;
}
