#include "meshCompiler.h"
#include <iostream>
#include <fstream>
#include <vector>

// function from: https://stackoverflow.com/questions/5888022/split-string-by-single-spaces
size_t split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != std::string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

    return strs.size();
}

#define mesh_compiler_unsigned_int 'u'
#define mesh_compiler_float 'f'
#define mesh_compiler_indice 'i'
#define mesh_compiler_vertex 'v'
#define mesh_compiler_normal 'n'
#define mesh_compiler_texture_coordinate 'c'
#define mesh_compiler_tangent 't'
#define mesh_compiler_bitangent 'b'

struct compileRecord {
    char type;
    char data[4];
};

void mesh_compiler::compile(const aiScene* scene, const compilationInfo& ci)
{
    if (scene->mNumMeshes == 1) {
        compileMesh(scene->mMeshes[0], ci);
        return;
    }
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        compilationInfo cii = ci;
        cii.name = ci.name + std::to_string(i);
        compileMesh(scene->mMeshes[i], cii);
    }
}

void mesh_compiler::compileMesh(const aiMesh* m, const compilationInfo& ci)
{
    // format file reading
    if (ci.debug_messages) {
        std::cout << "reading format file...\n";
    }
    std::ifstream formatFile(ci.format_file, std::ios::in);
    if (!formatFile) {
        std::cerr << "Cannot open file: " << ci.format_file << std::endl;
        return;
    }

    unsigned short int buffs = 0;
    std::vector<unsigned int> sizes;
    std::vector<std::vector<compileRecord>> records;

    std::string line;
    while (std::getline(formatFile, line)) {
        unsigned int siz = 0; // size of one entry, 4 bytes per 1 siz (if siz = 2 then 8 bytes)
        unsigned int count = 0; // amount of entries (calculated by assimp)

        std::vector<std::string> fields;
        size_t args = split(line, fields, ' '); // obtain fields
        if (args == 0) continue; // if no fields skip

        records.push_back(std::vector<compileRecord>());
        for (std::string field : fields) {
            if (field.empty()) continue; // if field invalid - skip

            compileRecord cr; // fill record with type and data
            cr.type = field[0]; // first character of the entry signifies type
            if (cr.type == mesh_compiler_float) {
                float val = std::stof(field.substr(2, field.size() - 2)); // remove first 2 because first is type second is '.'
                memcpy(cr.data, (char*)&val, sizeof(float));
            }
            else {
                unsigned int val = std::stof(field.substr(2, field.size() - 2));
                memcpy(cr.data, (char*)&val, sizeof(int));
            }

            // calculate count
            switch (cr.type)
            {
            case mesh_compiler_float:
            case mesh_compiler_unsigned_int:
                break;
            case mesh_compiler_indice:
                if (count != m->mNumFaces && count != 0) {
                    std::cout << "compilation format error: conflicting types in one buffer\n";
                    formatFile.close();
                    return;
                }
                count = m->mNumFaces;
                break;
            default:
                if (count != m->mNumVertices && count != 0) {
                    std::cout << "compilation format error: conflicting types in one buffer\n";
                    formatFile.close();
                    return;
                }
                count = m->mNumVertices;
                break;
            }

            records.back().push_back(cr);
            ++siz;
        }
        if (count == 0) {
            std::cout << "compilation format error: buffer of only constants - size unknown\n";
            formatFile.close();
            return;
        }
        sizes.push_back(siz * count);
        ++buffs;
    }

    if (ci.debug_messages) {
        std::cout << "Format file info:\n";
        std::cout << " buffs: " << buffs << "\n sizes:";
        for (const unsigned int& siz : sizes)
            std::cout << " " << siz;
        std::cout << "\n compile records:\n";
    }

    formatFile.close();

    // output file creation
    std::ofstream fout(ci.name + ci.extension, std::ios::out | std::ios::binary);
    if (!fout) {
        std::cerr << "Cannot open file: " << ci.name + ci.extension << std::endl;
        return;
    }
    
    fout.write((char*)&buffs, sizeof(unsigned short int));
    fout.write((char*)(sizes.data()), sizes.size() * sizeof(unsigned int));

    for (int i = 0; i < records.size(); ++i) {
        for (int j = 0; j < sizes[i] / records[i].size(); ++j) {
            for (const compileRecord& cr : records[i]) {
                unsigned int ind = 0;
                memcpy((char*)&ind, cr.data, sizeof(unsigned int));
                switch (cr.type)
                {
                case mesh_compiler_unsigned_int:
                case mesh_compiler_float:
                    fout.write(cr.data, sizeof(float));
                    break;
                case mesh_compiler_indice:
                    fout.write((char*)&(m->mFaces[j].mIndices[ind]), sizeof(unsigned int));
                    break;
                case mesh_compiler_vertex:
                    fout.write((char*)&(m->mVertices[j][ind]), sizeof(float));
                    break;
                case mesh_compiler_normal:
                    fout.write((char*)&(m->mNormals[j][ind]), sizeof(float));
                    break;
                case mesh_compiler_texture_coordinate:
                    fout.write((char*)&(m->mTextureCoords[0][j][ind]), sizeof(float));
                    break;
                case mesh_compiler_tangent:
                    fout.write((char*)&(m->mTangents[j][ind]), sizeof(float));
                    break;
                case mesh_compiler_bitangent:
                    fout.write((char*)&(m->mBitangents[j][ind]), sizeof(float));
                    break;
                default:
                    break;
                }
            }
        }
    }

    fout.close();
}

void mesh_compiler::compile_old(const aiScene* scene, const compilationInfo& ci)
{
    if (scene->mNumMeshes == 1) {
        compileMesh_old(scene->mMeshes[0], ci);
        return;
    }
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        compilationInfo cii = ci;
        cii.name = ci.name + std::to_string(i);
        compileMesh_old(scene->mMeshes[i], cii);
    }
}

void mesh_compiler::compileMesh_old(const aiMesh* m, const compilationInfo& ci)
{
    std::ofstream fout(ci.name + ci.extension, std::ios::out | std::ios::binary);
    if (!fout) {
        std::cerr << "Cannot open file: " << ci.name + ci.extension << std::endl;
        return;
    }

    unsigned short int buffs = 6;
    fout.write((char*)&buffs, sizeof(unsigned short int));

    std::vector<unsigned int> sizes;
    sizes.push_back(m->mNumFaces * 3);
    sizes.push_back(m->mNumVertices * 3);
    sizes.push_back(m->mNumVertices * 3);
    sizes.push_back(m->mNumVertices * 3);
    sizes.push_back(m->mNumVertices * 3);
    sizes.push_back(m->mNumVertices * 3);

    fout.write((char*)(sizes.data()), sizes.size() * sizeof(unsigned int));

    for (int i = 0; i < m->mNumFaces; ++i) {
        fout.write((char*)(m->mFaces->mIndices), m->mFaces->mNumIndices * sizeof(unsigned int));
    }
    fout.write((char*)(m->mVertices), m->mNumVertices * sizeof(aiVector3D));
    fout.write((char*)(m->mNormals), m->mNumVertices * sizeof(aiVector3D));
    fout.write((char*)(m->mTextureCoords[0]), m->mNumVertices * sizeof(aiVector3D));
    fout.write((char*)(m->mTangents), m->mNumVertices * sizeof(aiVector3D));
    fout.write((char*)(m->mBitangents), m->mNumVertices * sizeof(aiVector3D));

    fout.close();
}
