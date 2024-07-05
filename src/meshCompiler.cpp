#include "meshCompiler.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

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

// defines for compilePreambule::info_format[]
#define mesh_compiler_buffers_count '+' // amount of all buffers in the file
#define mesh_compiler_buffers_x_entries '@' // amount of all entries in the file
#define mesh_compiler_buffers_x_entries_x_fields '#' // amount of all fields in the file
#define mesh_compiler_buffer_byte_sizes 'a' // print sizes of all buffers (unsigned int - 4 bytes) x buffers_count
#define mesh_compiler_buffer_2byte_sizes 'b'
#define mesh_compiler_buffer_4byte_sizes 'c'
#define mesh_compiler_preambule_args 8

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

namespace mesh_compiler {
    struct compileField {
    public:
        char type;
        char data[4];

        unsigned int get_size(size_t byte_base = 1) const;
        void print(const int& indent = 0) const;
    };

    struct compileBuffer {
    public:
        std::vector<char> info_format;
        size_t count = 0;
        std::vector<compileField> fields;

        unsigned int get_size(size_t byte_base = 1) const;
        void print(const int& indent = 0) const;
    };

    struct compilePreambule {
        std::vector<char> info_format;

        void print(const int& indent = 0) const;
    };

    struct compileConfig {
        compilePreambule preambule;
        std::vector<compileBuffer> buffers;
        void print(const int& indent = 0) const;
    };

    char obtainCompileConfig(compileConfig& config, const compilationInfo& ci);
}

unsigned int mesh_compiler::compileField::get_size(size_t byte_base) const
{
    return 4 / byte_base;
}

void mesh_compiler::compileField::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    printf("field info: type: %c, data: %c%c%c%c\n", type, data[0], data[1], data[2], data[3]);
}

unsigned int mesh_compiler::compileBuffer::get_size(size_t byte_base) const
{
    unsigned int siz = 0;
    for (const compileField& cf : fields)
        siz += cf.get_size();
    return siz * count / byte_base;
}

void mesh_compiler::compileBuffer::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "buffer info: info format: ";
    for (const char& c : info_format) std::cout << c;
    std::cout << ", count: " << count << ", fields: \n";
    for (const compileField& f : fields) f.print(indent + 1);
}

void mesh_compiler::compilePreambule::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "preambule info: info format: ";
    for (const char& c : info_format) std::cout << c;
    std::cout << std::endl;
}

void mesh_compiler::compileConfig::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "config info:\n";
    preambule.print(indent + 1);
    for (const compileBuffer& b : buffers) b.print(indent + 1);
}

std::map<std::string, char> argsMap = {
    {"buffc", '+'},
    {"buffs4_4", 'a'}
};

std::vector<char> fieldsVec = { 'i', 'v', 'n', 't', 'b', 'c', '0', '1', '2', '3', '4', '5', '6', '7' };
std::vector<char> typesVec = { 'f' };
std::map<char, int> suffixesMap = {
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3},
    {'x', 0}, {'y', 1}, {'z', 2},
    {'r', 0}, {'g', 1}, {'b', 2}, {'a', 3},
    {'u', 0}, {'v', 1}, {'w', 2}
};

char mesh_compiler::obtainCompileConfig(compileConfig& config, const compilationInfo& ci)
{
    std::ifstream formatFile(ci.format_file, std::ios::in);
    if (!formatFile) {
        std::cout << "format compilation error: cannot open file: " << ci.format_file << std::endl;
        return 1;
    }

    std::string line, arg = "";

    // preambule
    std::getline(formatFile, line);
    line += ' ';
    for (const char& c : line) {
        if (c == ' ' || c == '\t' || c == '\n') {
            if (arg.empty()) continue;
            else {
                std::cout << arg << std::endl;
                if (argsMap.find(arg) != argsMap.end()) { // argument
                    config.preambule.info_format.push_back(argsMap[arg]);
                    arg = "";
                }
                else if (std::find(fieldsVec.begin(), fieldsVec.end(), arg[0]) != fieldsVec.end() || arg.size() <= 2) { // field
                    std::cout << "format compilation error: attempted field specification in preambule in line 1.\n";
                    return 5;
                }
                else {
                    std::cout << "format compilation error: unknown statement: " << arg << " in line 1.\n";
                    return 2;
                }
            }
        }
        else arg += c;
    }

    // buffers
    int line_num = 1;
    bool fields = false;
    while (std::getline(formatFile, line)) {
        line += ' ';
        compileBuffer buffer;
        char type = 'f';
        for (const char& c : line) {
            if ((c >= 9 && c <= 13) || c == ' ') { // whitespace character
                if (arg.empty()) continue;
                else { // process word
                    if (argsMap.find(arg) != argsMap.end()) { // argument
                        buffer.info_format.push_back(argsMap[arg]);
                        arg = "";
                    }
                    else if (std::find(fieldsVec.begin(), fieldsVec.end(), arg[0]) != fieldsVec.end()) { // field
                        if (arg.size() == 1) {
                            std::cout << "format compilation error: field suffix not provided: " << arg << "? in line " << line_num << ".\n";
                            return 3;
                        }
                        else if (suffixesMap.find(arg[1]) != suffixesMap.end()) {
                            compileField field;
                            field.type = arg[0];
                            if (field.type != type && type != 'f' && field.type != 'f') {
                                std::cout << "format compilation error: confilcting types detected in single buffer in line " << line_num << ".\n";
                                return 9;
                            }
                            if (field.type != 'f') type = field.type;
                            memcpy(field.data, &suffixesMap[arg[1]], sizeof(int));
                            buffer.fields.push_back(field);
                            arg = "";
                        }
                        else {
                            std::cout << "format compilation error: invalid field suffix: " << arg[1] << " in line " << line_num << ".\n";
                            return 4;
                        }
                    }
                    else if (std::find(typesVec.begin(), typesVec.end(), arg[0]) != typesVec.end()) { // constant
                        if (arg.size() == 1) {
                            std::cout << "format compilation error: constant value not provided: " << arg << "? in line " << line_num << ".\n";
                            return 3;
                        }
                        else {
                            compileField field;
                            field.type = arg[0];
                            float value = std::stof(arg.substr(1, arg.size() - 1));
                            memcpy(field.data, &value, sizeof(float));
                            buffer.fields.push_back(field);
                            arg = "";
                        }

                    }
                    else {
                        std::cout << "format compilation error: unknown statement: " << arg << " in line " << line_num << ".\n";
                        return 2;
                    }
                }
            }
            else arg += c;
        }
        if (type == 'f') {
            std::cout << "format compilation error: detected buffer of only constants - unknown buffer size in line " << line_num << ".\n";
            return 10;
        }
        config.buffers.push_back(buffer);
        ++line_num;
    }

    if (ci.debug_messages) config.print();

    formatFile.close();
    return 0;
}

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
    // obtain compile configuration
    compileConfig config;
    if (obtainCompileConfig(config, ci) != 0) {
        std::cout << "format file compilation ended with errors could not compile mesh\n";
        return;
    }

    // fill counts
    for (compileBuffer& buffer : config.buffers) {
        char type = 'f';
        for (const compileField& cf : buffer.fields) {
            if (cf.type != type && type != 'f' && cf.type != 'f') {
                printf("format error: confilcting types detected in single buffer: conflicting types: %c and %c\n", cf.type, type);
                return;
            }
            if (cf.type != 'f') type = cf.type;
        }
        if (type == 'f') {
            std::cout << "format error: detected buffer of only constants: unknown buffer size\n";
            return;
        }
        else if (type == 'i') buffer.count = m->mNumFaces;
        else buffer.count = m->mNumVertices;
    }

    // output file creation
    std::ofstream fout(ci.name + ci.extension, std::ios::out | std::ios::binary);
    if (!fout) {
        std::cout << "Cannot open file: " << ci.name + ci.extension << std::endl;
        return;
    }

    unsigned short int buffs = config.buffers.size();
    fout.write((char*)&buffs, sizeof(unsigned short int));
    for (const compileBuffer& cb : config.buffers) {
        unsigned int siz = cb.get_size(4);
        fout.write((char*)&siz, sizeof(unsigned int));
    }

    for (const compileBuffer& cb : config.buffers) {
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
