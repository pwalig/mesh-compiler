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

// defines for compileField::type
#define mesh_compiler_unsigned_int 'u'
#define mesh_compiler_float 'f'
#define mesh_compiler_indice 'i'
#define mesh_compiler_vertex 'v'
#define mesh_compiler_normal 'n'
#define mesh_compiler_texture_coordinate 'c'
#define mesh_compiler_uv0 '0'
#define mesh_compiler_uv1 '1'
#define mesh_compiler_uv2 '2'
#define mesh_compiler_uv3 '3'
#define mesh_compiler_uv4 '4'
#define mesh_compiler_uv5 '5'
#define mesh_compiler_uv6 '6'
#define mesh_compiler_uv7 '7'
#define mesh_compiler_tangent 't'
#define mesh_compiler_bitangent 'b'

struct compileField {
public:
    char type;
    char data[4];

    unsigned int get_size(size_t byte_base = 1) const;
};

unsigned int compileField::get_size(size_t byte_base) const
{
    return 4 / byte_base;
}

// defines for compileBuffer::info_format[] and compilePreambule::info_format[]
#define mesh_compiler_none ';'

// defines for compileBuffer::info_format[]
#define mesh_compiler_buffer_byte_size 'a' // size of the entire buffer (unsigned int - 4 bytes) x 1
#define mesh_compiler_buffer_2byte_size 'b'
#define mesh_compiler_buffer_4byte_size 'c'
#define mesh_compiler_entry_byte_size 'd' // size of the entire entry (sum of field sizes) (char - 1 byte) x 1
#define mesh_compiler_entry_2byte_size 'e'
#define mesh_compiler_entry_4byte_size 'f'
#define mesh_compiler_field_byte_sizes 'g' // print sizes of all fields one after another (char - 1 byte) x fields_count
#define mesh_compiler_field_2byte_sizes 'h'
#define mesh_compiler_field_4byte_sizes 'i'
#define mesh_compiler_fields_count '&' // amount of fields in single entry (char - 1 byte) x 1
#define mesh_compiler_entries_count '^' // amount of entries (calculated by assimp - eg. mNumVertices)
#define mesh_compiler_fields_x_entries_count '%' // amount of all fields in a buffer
#define mesh_compiler_buffer_args 12

struct compileBuffer {
public:
    char info_format[mesh_compiler_buffer_args];
    unsigned int count = 0;
    std::vector<compileField> fields;

    unsigned int get_size(size_t byte_base = 1) const;
};

unsigned int compileBuffer::get_size(size_t byte_base) const
{
    unsigned int siz = 0;
    for (const compileField& cf : fields)
        siz += cf.get_size();
    return siz * count / byte_base;
}

// defines for compilePreambule::info_format[]
#define mesh_compiler_buffers_count '+' // amount of all buffers in the file
#define mesh_compiler_buffers_x_entries '@' // amount of all entries in the file
#define mesh_compiler_buffers_x_entries_x_fields '#' // amount of all fields in the file
#define mesh_compiler_buffer_byte_sizes 'a' // print sizes of all buffers (unsigned int - 4 bytes) x buffers_count
#define mesh_compiler_buffer_2byte_sizes 'b'
#define mesh_compiler_buffer_4byte_sizes 'c'
#define mesh_compiler_preambule_args 8

struct compilePreambule {
    char info_format[mesh_compiler_preambule_args];
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
        std::cout << "compilation format error: cannot open file: " << ci.format_file << std::endl;
        return;
    }

    std::vector<compileBuffer> buffer_infos;
    compilePreambule cp;
    std::string line;

    // process preambule
    std::getline(formatFile, line);
    int i = 0;
    for (const char& c : line) {
        if (i >= mesh_compiler_preambule_args) {
            std::cout << "compilation format warning: too many parameters of the preambule: maximum " << mesh_compiler_preambule_args << " supprted, detected: " << line.size() << ", skipping last " << line.size() - mesh_compiler_preambule_args << std::endl;
            break;
        }
        cp.info_format[i] = c;
        ++i;
    }

    while (std::getline(formatFile, line)) {
        unsigned int siz = 0; // size of one entry, 4 bytes per 1 siz (if siz = 2 then 8 bytes)

        std::vector<std::string> fields;
        size_t args = split(line, fields, ' '); // obtain fields
        if (args == 0) continue; // if no fields skip

        compileBuffer buffer;

        // process compile buffer info
        int i = 0;
        for (const char& c : fields[0]) {
            if (i >= mesh_compiler_buffer_args) {
                std::cout << "compilation format warning: too many buffer parameters: maximum " << mesh_compiler_buffer_args << " supprted, detected: " << line.size() << ", skipping last " << line.size() - mesh_compiler_buffer_args << std::endl;
                break;
            }
            cp.info_format[i] = c;
            ++i;
        }

        for (int i = 1; i < fields.size(); ++i) {
            if (fields[i].empty()) continue; // if field invalid - skip

            compileField cf; // fill record with type and data
            cf.type = fields[i][0]; // first character of the entry signifies type
            if (cf.type == mesh_compiler_float) {
                float val = std::stof(fields[i].substr(2, fields[i].size() - 2)); // remove first 2 because first is type second is '.'
                memcpy(cf.data, (char*)&val, sizeof(float));
            }
            else {
                unsigned int val = std::stof(fields[i].substr(2, fields[i].size() - 2));
                memcpy(cf.data, (char*)&val, sizeof(int));
            }

            // calculate count
            switch (cf.type)
            {
            case mesh_compiler_float:
            case mesh_compiler_unsigned_int:
                break;
            case mesh_compiler_indice:
                if (buffer.count != m->mNumFaces && buffer.count != 0) {
                    std::cout << "compilation format error: conflicting types in one buffer\n";
                    formatFile.close();
                    return;
                }
                buffer.count = m->mNumFaces;
                break;
            default:
                if (buffer.count != m->mNumVertices && buffer.count != 0) {
                    std::cout << "compilation format error: conflicting types in one buffer\n";
                    formatFile.close();
                    return;
                }
                buffer.count = m->mNumVertices;
                break;
            }
            buffer.fields.push_back(cf);
            ++siz;
        }
        if (buffer.count == 0) {
            std::cout << "compilation format error: buffer of only constants - size unknown\n";
            formatFile.close();
            return;
        }
        buffer_infos.push_back(buffer);
    }

    if (ci.debug_messages) {
        std::cout << "Compilation info:\n";
        std::cout << " buffers: " << buffer_infos.size() << "\n sizes:";
    }

    formatFile.close();

    // output file creation
    std::ofstream fout(ci.name + ci.extension, std::ios::out | std::ios::binary);
    if (!fout) {
        std::cerr << "Cannot open file: " << ci.name + ci.extension << std::endl;
        return;
    }
    unsigned short int buffs = buffer_infos.size();
    fout.write((char*)&buffs, sizeof(unsigned short int));
    for (const compileBuffer& cb : buffer_infos) {
        unsigned int siz = cb.get_size(4);
        fout.write((char*)&siz, sizeof(unsigned int));
    }

    for (const compileBuffer& cb : buffer_infos) {
        if (ci.debug_messages) {
            std::cout << " buffer info:\n";
            std::cout << "  fields: " << cb.fields.size() * cb.count << ", size: " << cb.get_size() << std::endl;
        }
        for (unsigned int j = 0; j < cb.count; ++j) {
            for (const compileField& cf : cb.fields) {
                unsigned int ind = 0;
                memcpy((char*)&ind, cf.data, sizeof(unsigned int));
                switch (cf.type)
                {
                case mesh_compiler_unsigned_int:
                case mesh_compiler_float:
                    fout.write(cf.data, sizeof(float));
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
