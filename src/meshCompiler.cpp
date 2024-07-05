#include "meshCompiler.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

// defines for compileField::type
#define mc_unsigned_int 'u'
#define mc_float 'f'
#define mc_indice 'i'
#define mc_vertex 'v'
#define mc_normal 'n'
#define mc_texture_coordinate 'c'
#define mc_uv0 '0'
#define mc_uv1 '1'
#define mc_uv2 '2'
#define mc_uv3 '3'
#define mc_uv4 '4'
#define mc_uv5 '5'
#define mc_uv6 '6'
#define mc_uv7 '7'
#define mc_tangent 't'
#define mc_bitangent 'b'

// defines for compiler sub types (specifies type size and base)
#define mc_x_1 0b00000000
#define mc_x_2 0b00000001
#define mc_x_4 0b00000010
#define mc_x_8 0b00000011
#define mc_1_x 0b00000000
#define mc_2_x 0b00000100
#define mc_4_x 0b00001000
#define mc_8_x 0b00001100

#define mc_1_1 (mc_1_x | mc_x_1)
#define mc_1_2 (mc_1_x | mc_x_2)
#define mc_1_4 (mc_1_x | mc_x_4)
#define mc_1_8 (mc_1_x | mc_x_8)
#define mc_2_1 (mc_2_x | mc_x_1)
#define mc_2_2 (mc_2_x | mc_x_2)
#define mc_2_4 (mc_2_x | mc_x_4)
#define mc_2_8 (mc_2_x | mc_x_8)
#define mc_4_1 (mc_4_x | mc_x_1)
#define mc_4_2 (mc_4_x | mc_x_2)
#define mc_4_4 (mc_4_x | mc_x_4)
#define mc_4_8 (mc_4_x | mc_x_8)
#define mc_8_1 (mc_8_x | mc_x_1)
#define mc_8_2 (mc_8_x | mc_x_2)
#define mc_8_4 (mc_8_x | mc_x_4)
#define mc_8_8 (mc_8_x | mc_x_8)

#define mc_mask 0b11110000
#define mc_x_mask mc_x_8
#define mc_mask_x mc_8_x
#define mc_mask_mask (mc_mask_x | mc_x_mask)

// defines for compileBuffer::info_format[] and compilePreambule::info_format[]
#define mc_buffer 0b01000000 // size of the entire buffer
#define mc_entry 0b00100000 // size of the entire entry (sum of field sizes)
#define mc_field 0b00010000 // print sizes of all fields one after another
#define mc_fields_count 0b10010000 // amount of fields in single entry (char - 1 byte) x 1
#define mc_entries_count 0b10100000 // amount of entries (calculated by assimp - eg. mNumVertices)
#define mc_entries_x_fields 0b10110000 // amount of all fields in a buffer
#define mc_buffers_count 0b11000000 // amount of all buffers in the file
#define mc_buffers_x_entries 0b11100000 // amount of all entries in the file
#define mc_buffers_x_entries_x_fields 0b11110000 // amount of all fields in the file

namespace mesh_compiler {
    struct compileField {
    public:
        char type;
        char data[4];

        unsigned int get_size(unsigned short byte_base = 1) const;
        void print(const int& indent = 0) const;
    };

    struct compileBuffer {
    public:
        std::vector<char> info_format;
        size_t count = 0;
        std::vector<compileField> fields;

        unsigned int get_entry_size(unsigned short byte_base = 1) const;
        unsigned int get_size(unsigned short byte_base = 1) const;
        void print(const int& indent = 0) const;
    };

    struct compilePreambule {
        std::vector<char> info_format;

        void print(const int& indent = 0) const;
    };

    struct compileConfig {
        compilePreambule preambule;
        std::vector<compileBuffer> buffers;

        unsigned int get_size(unsigned short byte_base = 1);
        unsigned int get_entries_count();
        unsigned int get_fields_count();
        void print(const int& indent = 0) const;
    };

    char obtainCompileConfig(compileConfig& config, const compilationInfo& ci);
}

unsigned int mesh_compiler::compileField::get_size(unsigned short byte_base) const
{
    return 4 / byte_base;
}

void mesh_compiler::compileField::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    printf("field info: type: %c, data: %c%c%c%c\n", type, data[0], data[1], data[2], data[3]);
}

unsigned int mesh_compiler::compileBuffer::get_entry_size(unsigned short byte_base) const
{
    unsigned int siz = 0;
    for (const compileField& cf : fields)
        siz += cf.get_size();
    return siz / byte_base;
}

unsigned int mesh_compiler::compileBuffer::get_size(unsigned short byte_base) const
{
    return get_entry_size() * count / byte_base;
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

unsigned int mesh_compiler::compileConfig::get_size(unsigned short byte_base)
{
    unsigned int siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.get_size();
    return siz;
}

unsigned int mesh_compiler::compileConfig::get_entries_count()
{
    unsigned int siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.count;
    return siz;
}

unsigned int mesh_compiler::compileConfig::get_fields_count()
{
    unsigned int siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.fields.size();
    return siz;
}

void mesh_compiler::compileConfig::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "config info:\n";
    preambule.print(indent + 1);
    for (const compileBuffer& b : buffers) b.print(indent + 1);
}

std::map<std::string, char> argsMap = {
    {"buffc", mc_buffers_count },
    {"buffs", mc_buffer },
    {"entrya", mc_buffers_x_entries },
    {"entryc", mc_entries_count },
    {"entrys", mc_entry },
    {"fielda", mc_buffers_x_entries_x_fields },
    {"fielde", mc_entries_x_fields },
    {"fieldc", mc_fields_count },
    {"fields", mc_field },
};

std::map<char, char> sizeMap = {
    {'1', mc_1_x },
    {'2', mc_2_x },
    {'4', mc_4_x },
    {'8', mc_8_x }
};

std::map<char, char> baseMap = {
    {'1', mc_x_1 },
    {'2', mc_x_2 },
    {'4', mc_x_4 },
    {'8', mc_x_8 }
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
        if ((c >= 9 && c <= 13) || c == ' ') {
            if (arg.empty()) continue;
            else { // process word
                if (arg.size() >= 4 && argsMap.find(arg.substr(0, arg.size()-3)) != argsMap.end()) { // argument
                    char varg = argsMap[arg.substr(0, arg.size() - 3)];
                    char size = sizeMap[arg[arg.size() - 3]];
                    char base = baseMap[arg[arg.size() - 1]];
                    config.preambule.info_format.push_back(varg | size | base);
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
                    if (arg.size() >= 4 && argsMap.find(arg.substr(0, arg.size() - 3)) != argsMap.end()) { // argument
                        char varg = argsMap[arg.substr(0, arg.size() - 3)];
                        char size = sizeMap[arg[arg.size() - 3]];
                        char base = baseMap[arg[arg.size() - 1]];
                        buffer.info_format.push_back(varg | size | base);
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

template <typename T>
void writeConst(std::ofstream& file, const T& value) {
    T x = value;
    file.write((char*)&x, sizeof(T));
}

template <typename T>
void writeConst(std::ofstream& file, const T& value, const char& type) {
    if ((type & mc_mask_x) == mc_1_x) writeConst<char>(file, value);
    else if ((type & mc_mask_x) == mc_2_x) writeConst<unsigned short>(file, value);
    else if ((type & mc_mask_x) == mc_4_x) writeConst<unsigned int>(file, value);
    else if ((type & mc_mask_x) == mc_8_x) writeConst<unsigned long>(file, value);
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
        std::cout << "compilation error: cannot open file: " << ci.name + ci.extension << std::endl;
        return;
    }

    // preambule
    for (const char& c : config.preambule.info_format) {

        // base
        unsigned short base = 1;
        if ((c & mc_x_mask) == mc_x_2) base = 2;
        else if ((c & mc_x_mask) == mc_x_4) base = 4;
        else if ((c & mc_x_mask) == mc_x_8) base = 8;

        // size
        switch (c & mc_mask)
        {
        case mc_buffer:
            for (const compileBuffer& cb : config.buffers) {
                writeConst(fout, cb.get_size(base), c);
            }
            break;
        case mc_buffers_count:
            writeConst(fout, config.buffers.size(), c);
            break;
        case mc_entry:
            for (const compileBuffer& cb : config.buffers) {
                writeConst(fout, cb.get_entry_size(base), c);
            }
            break;
        case mc_entries_count:
            for (const compileBuffer& cb : config.buffers) {
                writeConst(fout, cb.count, c);
            }
            break;
        case mc_buffers_x_entries:
            writeConst(fout, config.get_entries_count(), c);
            break;
        case mc_field:
            for (const compileBuffer& cb : config.buffers) {
                for (const compileField& cf : cb.fields) {
                    writeConst(fout, cf.get_size(base), c);
                }
            }
            break;
        case mc_fields_count:
            for (const compileBuffer& cb : config.buffers) {
                writeConst(fout, cb.fields.size(), c);
            }
            break;
        case mc_entries_x_fields:
            for (const compileBuffer& cb : config.buffers) {
                writeConst(fout, cb.fields.size() * cb.count, c);
            }
            break;
        case mc_buffers_x_entries_x_fields:
            writeConst(fout, config.get_fields_count(), c);
            break;
        default:
            printf("compilation warning: unknown buffer info flag: %c skipping the flag\n", c);
            break;
        }
    }

    // buffers
    for (const compileBuffer& cb : config.buffers) {

        // format info
        for (const char& c : cb.info_format) {

            // base
            unsigned short base = 1;
            if ((c & mc_x_mask) == mc_x_2) base = 2;
            else if ((c & mc_x_mask) == mc_x_4) base = 4;
            else if ((c & mc_x_mask) == mc_x_8) base = 8;

            // size
            switch (c & mc_mask)
            {
            case mc_buffer:
                writeConst(fout, cb.get_size(base), c);
                break;
            case mc_entry:
                writeConst(fout, cb.get_entry_size(base), c);
                break;
            case mc_entries_count:
                writeConst(fout, cb.count, c);
                break;
            case mc_field:
                for (const compileField& cf : cb.fields) {
                    writeConst(fout, cf.get_size(base), c);
                }
                break;
            case mc_fields_count:
                writeConst(fout, cb.fields.size(), c);
                break;
            case mc_entries_x_fields:
                writeConst(fout, cb.fields.size() * cb.count, c);
                break;
            default:
                printf("compilation warning: unknown buffer info flag: %c skipping the flag\n", c);
                break;
            }
        }

        // data buffers
        for (unsigned int j = 0; j < cb.count; ++j) {
            for (const compileField& cf : cb.fields) {
                unsigned int ind = 0;
                memcpy((char*)&ind, cf.data, sizeof(unsigned int));
                switch (cf.type)
                {
                case mc_unsigned_int:
                case mc_float:
                    fout.write(cf.data, sizeof(float));
                    break;
                case mc_indice:
                    fout.write((char*)&(m->mFaces[j].mIndices[ind]), sizeof(unsigned int));
                    break;
                case mc_vertex:
                    fout.write((char*)&(m->mVertices[j][ind]), sizeof(float));
                    break;
                case mc_normal:
                    fout.write((char*)&(m->mNormals[j][ind]), sizeof(float));
                    break;
                case mc_texture_coordinate:
                    fout.write((char*)&(m->mTextureCoords[0][j][ind]), sizeof(float));
                    break;
                case mc_tangent:
                    fout.write((char*)&(m->mTangents[j][ind]), sizeof(float));
                    break;
                case mc_bitangent:
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
