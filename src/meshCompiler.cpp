#include "meshCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <assimpReader.h>

// ========== DEFINES AND MAPS ==========

// defines for compileField::type
// constant types
#define mc_char 'J'
#define mc_short 'S'
#define mc_int 'I'
#define mc_long 'L'
#define mc_unsigned_short 'T'
#define mc_unsigned_int 'U'
#define mc_unsigned_long 'M'
#define mc_float 'f'
#define mc_double 'd'
#define mc_long_double 'l'
// variable types
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
#define mc_vertex_color0 'A'
#define mc_vertex_color1 'B'
#define mc_vertex_color2 'C'
#define mc_vertex_color3 'D'
#define mc_vertex_color4 'E'
#define mc_vertex_color5 'F'
#define mc_vertex_color6 'G'
#define mc_vertex_color7 'H'

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

// defines for compileBuffer::info_format[] and compilePreamble::info_format[]
#define mc_constant 0b00000000 // constant value of any type
#define mc_buffer 0b01000000 // size of the entire buffer
#define mc_entry 0b00100000 // size of the entire entry (sum of field sizes)
#define mc_field 0b00010000 // print sizes of all fields one after another
#define mc_fields_count 0b10010000 // amount of fields in single entry (char - 1 byte) x 1
#define mc_entries_count 0b10100000 // amount of entries (calculated by assimp - eg. mNumVertices)
#define mc_entries_x_fields 0b10110000 // amount of all fields in a buffer
#define mc_buffers_count 0b11000000 // amount of all buffers in the file
#define mc_buffers_x_entries 0b11100000 // amount of all entries in the file
#define mc_buffers_x_entries_x_fields 0b11110000 // amount of all fields in the file

// error defines
#define mc_no_err 0
#define mc_err_cannot_open_file 1
#define mc_err_unknown_statement 2
#define mc_err_no_suffix 3
#define mc_err_invalid_suffix 4
#define mc_err_no_const_value 5
#define mc_err_byte_base_in_count_type 6
#define mc_err_field_spec_in_preamble 7
#define mc_err_unsupported_type 8
#define mc_err_conflicting_fields 9
#define mc_err_constants_only 10

#define mc_fit 0
#define mc_no_fit 255

std::map<int, std::string> errorMessageMap = {
    {mc_err_cannot_open_file, "could not open file"},
    {mc_err_unknown_statement, "unknown statement"},
    {mc_err_no_suffix, "field suffix not provided"},
    {mc_err_invalid_suffix, "invalid field suffix"},
    {mc_err_no_const_value, "constant value not provided"},
    {mc_err_byte_base_in_count_type, "given byte base in count type"},
    {mc_err_field_spec_in_preamble, "attempted field specification in preamble"},
    {mc_err_unsupported_type, "unsupported type"},
    {mc_err_conflicting_fields, "confilcting types detected in single buffer"},
    {mc_err_constants_only, "detected buffer of only constants - unknown buffer size"}
};

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

std::vector<char> fieldsVec = {
    mc_indice,
    mc_vertex,
    mc_normal,
    mc_texture_coordinate,
    mc_uv0,
    mc_uv1,
    mc_uv2,
    mc_uv3,
    mc_uv4,
    mc_uv5,
    mc_uv6,
    mc_uv7,
    mc_tangent,
    mc_bitangent,
    mc_vertex_color0,
    mc_vertex_color1,
    mc_vertex_color2,
    mc_vertex_color3,
    mc_vertex_color4,
    mc_vertex_color5,
    mc_vertex_color6,
    mc_vertex_color7
};

std::map<char, unsigned short> typeSizesMap = {
    {mc_char, sizeof(char)},
    {mc_short, sizeof(short)},
    {mc_int, sizeof(int)},
    {mc_long, sizeof(long)},
    {mc_unsigned_short, sizeof(unsigned short)},
    {mc_unsigned_int, sizeof(unsigned int)},
    {mc_unsigned_long, sizeof(unsigned long)},
    {mc_float, sizeof(float)},
    {mc_double, sizeof(double)},
    {mc_long_double, sizeof(long double)}
};

char getFieldCount(char fieldType) {
    switch (fieldType)
    {
    case 'v':
    case 'n':
    case 't':
    case 'b':
    case 'c':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
        return 'v';
    case 'i':
        return 'i';
    default:
        if (typeSizesMap.find(fieldType) != typeSizesMap.end()) return 0;
        return 128;
        break;
    }
}

std::map<char, int> suffixesMap = {
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3},
    {'x', 0}, {'y', 1}, {'z', 2},
    {'r', 0}, {'g', 1}, {'b', 2}, {'a', 3},
    {'u', 0}, {'v', 1}, {'w', 2}
};

void copyConstantToMemory(void* dst, const char& type, const std::string& val) {
    if (type == mc_char) {
        memcpy(dst, &val[0], typeSizesMap[type]);
    }
    else if (type == mc_short) {
        short value = std::stoi(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_int) {
        int value = std::stoi(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_long) {
        long value = std::stol(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_unsigned_short) {
        unsigned short value = std::stoi(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_unsigned_int) {
        unsigned int value = std::stoi(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_unsigned_long) {
        unsigned long value = std::stol(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_float) {
        float value = std::stof(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_double) {
        double value = std::stod(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
    else if (type == mc_long_double) {
        long double value = std::stold(val);
        memcpy(dst, &value, typeSizesMap[type]);
    }
}

// ========== METHODS DEFINITIONS ==========

unsigned int mesh_compiler::compileField::get_size(unsigned short byte_base) const
{
    return 4 / byte_base;
}

void mesh_compiler::compileField::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    printf("field info: type: %c, data: %c%c%c%c\n", type, data[0], data[1], data[2], data[3]);
}

void mesh_compiler::compilePreamble::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "info format: ";
    for (const char& c : info_format) std::cout << c;
    std::cout << ", data: ";
    for (const char& c : data) std::cout << c;
}

void mesh_compiler::compilePreamble::clear()
{
    this->info_format.clear();
    this->data.clear();
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
    std::cout << "buffer info: ";
    this->preamble.print(0);
    std::cout << ", count: " << count << ", fields: \n";
    for (const compileField& f : fields) f.print(indent + 1);
}

void mesh_compiler::compileBuffer::clear()
{
    this->preamble.clear();
    this->fields.clear();
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
    std::cout << " preamble info: ";
    preamble.print(0);
    std::cout << "\n";
    for (const compileBuffer& b : buffers) b.print(indent + 1);
}

void mesh_compiler::compileConfig::clear()
{
    this->preamble.clear();
    this->buffers.clear();
}

void mesh_compiler::compileConfig::logCompilationError(const int& err_code, const std::string& arg, const int& line_num)
{
    std::cout << "format compilation error: " << errorMessageMap[err_code] << ": " << arg << " in line " << line_num << "." << this->error_message << "\n";
}

int mesh_compiler::compileConfig::isArgument(const std::string& arg, compilePreamble& preamble)
{
    if (arg.size() >= 4 && argsMap.find(arg.substr(0, arg.size() - 3)) != argsMap.end()) { // argument
        if (arg[arg.size() - 2] != '_') {
            std::string correct = arg;
            correct[correct.size() - 2] = '_';
            this->error_message = " did you mean: " + correct + "?";
            return mc_err_unknown_statement;
        }
        if (arg[arg.size() - 4] != 's') {
            return mc_err_byte_base_in_count_type;
        }
        char varg = argsMap[arg.substr(0, arg.size() - 3)];
        char size = arg[arg.size() - 3];
        char base = arg[arg.size() - 1];
        if (sizeMap.find(size) == sizeMap.end() || baseMap.find(base) == baseMap.end()) {
            return mc_err_unknown_statement;
        }
        size = sizeMap[size];
        base = baseMap[base];
        preamble.info_format.push_back(varg | size | base);
        return mc_fit;
    }
    else if (arg.size() >= 2 && argsMap.find(arg.substr(0, arg.size() - 1)) != argsMap.end()) {
        char varg = argsMap[arg.substr(0, arg.size() - 1)];
        char size = arg[arg.size() - 1];
        if (sizeMap.find(size) == sizeMap.end()) {
            return mc_err_unknown_statement;
        }
        size = sizeMap[size];
        preamble.info_format.push_back(varg | size | mc_x_4);
        return mc_fit;
    }
    else if (argsMap.find(arg) != argsMap.end()) {
        char varg = argsMap[arg];
        preamble.info_format.push_back(varg | mc_4_4);
        return mc_fit;
    }
    return mc_no_fit;
}

int mesh_compiler::compileConfig::isField(const std::string& arg, std::vector<compileField>& fields, char& field_count)
{
    if (std::find(fieldsVec.begin(), fieldsVec.end(), arg[0]) != fieldsVec.end()) { // field
        if (arg.size() == 1) {
            return mc_err_no_suffix;
        }
        else if (suffixesMap.find(arg[1]) != suffixesMap.end()) {
            compileField field;
            field.type = arg[0];
            char ffc = getFieldCount(field.type);
            if (ffc != 0) {
                if (ffc != field_count && field_count != 0) {
                    this->error_message = " conflicting types: ";
                    this->error_message.push_back(field.type);
                    this->error_message += " and ";
                    this->error_message.push_back(field_count);
                    this->error_message += ".";
                    return mc_err_conflicting_fields;
                }
                field_count = ffc;
            }
            memcpy(field.data, &suffixesMap[arg[1]], sizeof(int));
            fields.push_back(field);
            return mc_fit;
        }
        else {
            return mc_err_invalid_suffix;
        }
    }
    return mc_no_fit;
}

int mesh_compiler::compileConfig::isType(const std::string& arg, compilePreamble& preamble)
{
    if (typeSizesMap.find(arg[0]) != typeSizesMap.end()) { // constant
        if (arg.size() == 1) {
            return mc_err_no_const_value;
        }
        else {
            int siz = typeSizesMap[arg[0]];
            preamble.info_format.push_back((char)siz);

            preamble.data.resize(preamble.data.size() + siz);

            copyConstantToMemory(&(preamble.data[preamble.data.size() - siz]), arg[0], arg.substr(1, arg.size() - 1));

            return mc_fit;
        }
    }
    return mc_no_fit;
}

int mesh_compiler::compileConfig::isType(const std::string& arg, std::vector<compileField>& fields)
{
    if (typeSizesMap.find(arg[0]) != typeSizesMap.end()) { // constant
        if (arg.size() == 1) {
            return mc_err_no_const_value;
        }
        else {
            compileField field;
            field.type = arg[0];
            if (typeSizesMap[arg[0]] != sizeof(float)) {
                this->error_message = " only 4 byte types are supported in field definitions.";
                return mc_err_unsupported_type;
            }
            copyConstantToMemory(field.data, arg[0], arg.substr(1, arg.size() - 1));
            fields.push_back(field);
            return mc_fit;
        }
    }
    return mc_no_fit;
}

int mesh_compiler::compileConfig::compile(const std::string& filename)
{
    std::ifstream formatFile(filename, std::ios::in);
    if (!formatFile) {
        std::cout << "format compilation error: cannot open file: " << filename << std::endl;
        return mc_err_cannot_open_file;
    }

    std::string line, arg = "";

    // preamble
    std::getline(formatFile, line);
    line += ' ';
    for (const char& c : line) {
        if ((c >= 9 && c <= 13) || c == ' ') {
            if (arg.empty()) continue;
            else { // process word

                int res = isArgument(arg, this->preamble);
                if (res == mc_fit) { arg = ""; continue; }

                if (res != mc_no_fit) { // compilation error
                    logCompilationError(res, arg, 1);
                    return res;
                }

                res = isType(arg, this->preamble);
                if (res == mc_fit) { arg = ""; continue; }

                if (res != mc_no_fit) { // compilation error
                    logCompilationError(res, arg, 1);
                    return res;
                }

                logCompilationError(mc_err_unknown_statement, arg, 1);
                return mc_err_unknown_statement;
            }
        }
        else arg += c;
    }

    // buffers
    int line_num = 2;
    while (std::getline(formatFile, line)) {
        line += ' ';
        compileBuffer buffer;
        char field_count = 0;
        for (const char& c : line) {
            if ((c >= 9 && c <= 13) || c == ' ') { // whitespace character
                if (arg.empty()) continue;
                else { // process word

                    if (field_count == 0) { // preamble

                        int res = isArgument(arg, buffer.preamble);
                        if (res == mc_fit) { arg = ""; continue; }

                        if (res != mc_no_fit) { // compilation error
                            logCompilationError(res, arg, line_num);
                            return res;
                        }

                        res = isField(arg, buffer.fields, field_count);
                        if (res == mc_fit) { arg = ""; continue; }

                        if (res != mc_no_fit) { // compilation error
                            logCompilationError(res, arg, line_num);
                            return res;
                        }

                        res = isType(arg, buffer.preamble);
                        if (res == mc_fit) { arg = ""; continue; }

                        if (res != mc_no_fit) { // compilation error
                            logCompilationError(res, arg, line_num);
                            return res;
                        }

                        logCompilationError(mc_err_unknown_statement, arg, line_num);
                        return mc_err_unknown_statement;
                    }

                    else { // fields

                        int res = isField(arg, buffer.fields, field_count);
                        if (res == mc_fit) { arg = ""; continue; }

                        if (res != mc_no_fit) { // compilation error
                            logCompilationError(res, arg, line_num);
                            return res;
                        }

                        res = isType(arg, buffer.fields);
                        if (res == mc_fit) { arg = ""; continue; }

                        if (res != mc_no_fit) { // compilation error
                            logCompilationError(res, arg, line_num);
                            return res;
                        }

                        logCompilationError(mc_err_unknown_statement, arg, line_num);
                        return mc_err_unknown_statement;
                    }
                }
            }
            else arg += c;
        }
        if (field_count == 0) {
            logCompilationError(mc_err_constants_only, "", line_num);
            return mc_err_constants_only;
        }
        this->buffers.push_back(buffer);
        ++line_num;
    }

    formatFile.close();
    std::cout << "format file compilation succeded\n";
    this->error_message = "";
    return mc_no_err;
}

int mesh_compiler::compilationInfo::updateCompileConfig()
{

    if (this->config != nullptr) {
        delete this->config;
    }

    this->config = new compileConfig();
    int res = this->config->compile(this->format_file);
    if (res != 0) return res;

    if (this->debug_messages) this->config->print();

    return res;
}

mesh_compiler::compilationInfo::~compilationInfo()
{
    if (this->config) delete this->config;
}

// ========== COMPILER FUNCTION DEFINITIONS ==========

void mesh_compiler::run(int argc, char** argv)
{
    if (argc == 1) {
        std::string line = "";
        while (1) {
            std::cout << "> ";
            std::getline(std::cin, line);
            if (line == "q") return;
            std::vector<std::string> args;
            std::stringstream ss(line);
            while (ss) {
                args.push_back(std::string());
                ss >> args.back();
            }
            compile(args);
        }
    }
    else {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        compile(args);
    }
}

int mesh_compiler::compile(const std::vector<std::string>& args)
{
    int siz = args.size();
    if (siz == 0) {
        std::cout << "error: source file not specified.\n";
        return 2;
    }

    compilationInfo ci;

    for (int i = 1; i < siz; ++i) {
        if (args[i] == "-f") {
            ++i;
            if (i == siz) {
                std::cout << "error: unspecified format file: -f <format file path>\n";
                return 2;
            }
            ci.format_file = args[i];
        }
        else if (args[i] == "-o") {
            ++i;
            if (i == siz) {
                std::cout << "error: output file: -o <output file path>\n";
                return 2;
            }
            ci.output_file = args[i];
        }
        else if (i == 1) ci.format_file = args[i];
        else if (i == 2) ci.output_file = args[i];
    }

    return compileFile(args[0], ci);
}

int mesh_compiler::compileFile(const std::string& filename, compilationInfo& ci)
{
    // replace {file} with file name in output file name
    size_t found = ci.output_file.find("{file}");
    if (found != std::string::npos) {
        std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
        size_t const p(base_filename.find_last_of('.'));
        ci.output_file.replace(found, 6, base_filename.substr(0, p));
    }

    assimp::readFile(filename, std::bind(compileScene, std::placeholders::_1, std::ref(ci)));
    return 0;
}

int mesh_compiler::compileScene(const aiScene* scene, compilationInfo& ci)
{
    // obtain compile configuration
    if (ci.config == nullptr) {
        if (ci.updateCompileConfig() != 0) {
            std::cout << "format file compilation ended with errors could not compile scene\n";
            return mc_err_cannot_open_file;
        }
    }

    // replace {scene} with scene name in output file name
    size_t found = ci.output_file.find("{scene}");
    if (found != std::string::npos) ci.output_file.replace(found, 7, scene->mName.C_Str());

    // find name and extension
    std::string orig_name = ci.output_file;
    int errors = 0;
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        size_t found = ci.output_file.find("{mesh}");
        if (found != std::string::npos) ci.output_file.replace(found, 6, scene->mMeshes[i]->mName.C_Str());
        if (compileMesh(scene->mMeshes[i], ci) != 0) {
            std::cout << "compilation of mesh: " << scene->mMeshes[i]->mName.C_Str() << " ended up with errors.\n";
            errors += 1;
        }
        ci.output_file = orig_name; // go back to original name
    }

    if (errors != 0) {
        std::cout << "scene compilation ended with errors\n";
        printf("compiled %d out of %d meshes\n", scene->mNumMeshes - errors, scene->mNumMeshes);
        return 2;
    }
    std::cout << "scene compilation ended with success\n";
    return mc_no_err;
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

int mesh_compiler::compileMesh(const aiMesh* m, compilationInfo& ci)
{
    // obtain compile configuration
    if (ci.config == nullptr) {
        if (ci.updateCompileConfig() != 0) {
            std::cout << "format file compilation ended with errors could not compile mesh\n";
            return 2;
        }
    }

    // fill counts
    for (compileBuffer& buffer : ci.config->buffers) {
        char field_count = 0;
        for (const compileField& cf : buffer.fields) {
            char ffc = getFieldCount(cf.type);
            if (ffc != 0) {
                if (ffc != field_count && field_count != 0) {
                    printf("format error: confilcting types detected in single buffer: conflicting types: %c and %c\n", cf.type, field_count);
                    return mc_err_conflicting_fields;
                }
                field_count = ffc;
            }
        }
        if (field_count == 0) {
            std::cout << "format error: detected buffer of only constants: unknown buffer size\n";
            return mc_err_constants_only;
        }
        else if (field_count == 'i') buffer.count = m->mNumFaces;
        else if (field_count == 'v') buffer.count = m->mNumVertices;
        else {
            std::cout << "format error: unknown field type count\n";
            return 35;
        }
    }

    // output file creation
    std::ofstream fout(ci.output_file, std::ios::out | std::ios::binary);
    if (!fout) {
        std::cout << "compilation error: cannot open file: " << ci.output_file << std::endl;
        return mc_err_cannot_open_file;
    }

    size_t data_pointer = 0;
    // preamble
    for (const char& c : ci.config->preamble.info_format) {

        // base
        unsigned short base = 1;
        if ((c & mc_x_mask) == mc_x_2) base = 2;
        else if ((c & mc_x_mask) == mc_x_4) base = 4;
        else if ((c & mc_x_mask) == mc_x_8) base = 8;

        // size
        switch (c & mc_mask)
        {
        case mc_constant:
            fout.write(&(ci.config->preamble.data[data_pointer]), c & mc_mask_mask);
            data_pointer += c & mc_mask_mask;
            break;
        case mc_buffer:
            for (const compileBuffer& cb : ci.config->buffers) {
                writeConst(fout, cb.get_size(base), c);
            }
            break;
        case mc_buffers_count:
            writeConst(fout, ci.config->buffers.size(), c);
            break;
        case mc_entry:
            for (const compileBuffer& cb : ci.config->buffers) {
                writeConst(fout, cb.get_entry_size(base), c);
            }
            break;
        case mc_entries_count:
            for (const compileBuffer& cb : ci.config->buffers) {
                writeConst(fout, cb.count, c);
            }
            break;
        case mc_buffers_x_entries:
            writeConst(fout, ci.config->get_entries_count(), c);
            break;
        case mc_field:
            for (const compileBuffer& cb : ci.config->buffers) {
                for (const compileField& cf : cb.fields) {
                    writeConst(fout, cf.get_size(base), c);
                }
            }
            break;
        case mc_fields_count:
            for (const compileBuffer& cb : ci.config->buffers) {
                writeConst(fout, cb.fields.size(), c);
            }
            break;
        case mc_entries_x_fields:
            for (const compileBuffer& cb : ci.config->buffers) {
                writeConst(fout, cb.fields.size() * cb.count, c);
            }
            break;
        case mc_buffers_x_entries_x_fields:
            writeConst(fout, ci.config->get_fields_count(), c);
            break;
        default:
            printf("compilation warning: unknown buffer info flag: %c skipping the flag\n", c);
            break;
        }
    }

    // buffers
    for (const compileBuffer& cb : ci.config->buffers) {

        // format info
        data_pointer = 0;
        for (const char& c : cb.preamble.info_format) {

            // base
            unsigned short base = 1;
            if ((c & mc_x_mask) == mc_x_2) base = 2;
            else if ((c & mc_x_mask) == mc_x_4) base = 4;
            else if ((c & mc_x_mask) == mc_x_8) base = 8;

            // size
            switch (c & mc_mask)
            {
            case mc_constant:
                fout.write(&(cb.preamble.data[data_pointer]), c & mc_mask_mask);
                data_pointer += c & mc_mask_mask;
                break;
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
                case mc_int:
                case mc_unsigned_int:
                case mc_float:
                    fout.write(cf.data, sizeof(float));
                    break;
                case mc_indice:
                    fout.write((char*)&(m->mFaces[j].mIndices[ind]), sizeof(unsigned int));
                    break;
                case mc_vertex:
                    fout.write((char*)&(m->mVertices[j][ind]), sizeof(ai_real));
                    break;
                case mc_normal:
                    fout.write((char*)&(m->mNormals[j][ind]), sizeof(ai_real));
                    break;
                case mc_texture_coordinate:
                case mc_uv0:
                    fout.write((char*)&(m->mTextureCoords[0][j][ind]), sizeof(ai_real));
                    break;
                case mc_uv1:
                    fout.write((char*)&(m->mTextureCoords[1][j][ind]), sizeof(ai_real));
                    break;
                case mc_uv2:
                    fout.write((char*)&(m->mTextureCoords[2][j][ind]), sizeof(ai_real));
                    break;
                case mc_uv3:
                    fout.write((char*)&(m->mTextureCoords[3][j][ind]), sizeof(ai_real));
                    break;
                case mc_uv4:
                    fout.write((char*)&(m->mTextureCoords[4][j][ind]), sizeof(ai_real));
                    break;
                case mc_uv5:
                    fout.write((char*)&(m->mTextureCoords[5][j][ind]), sizeof(ai_real));
                    break;
                case mc_uv6:
                    fout.write((char*)&(m->mTextureCoords[6][j][ind]), sizeof(ai_real));
                    break;
                case mc_uv7:
                    fout.write((char*)&(m->mTextureCoords[7][j][ind]), sizeof(ai_real));
                    break;
                case mc_tangent:
                    fout.write((char*)&(m->mTangents[j][ind]), sizeof(ai_real));
                    break;
                case mc_bitangent:
                    fout.write((char*)&(m->mBitangents[j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color0:
                    fout.write((char*)&(m->mColors[0][j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color1:
                    fout.write((char*)&(m->mColors[1][j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color2:
                    fout.write((char*)&(m->mColors[2][j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color3:
                    fout.write((char*)&(m->mColors[3][j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color4:
                    fout.write((char*)&(m->mColors[4][j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color5:
                    fout.write((char*)&(m->mColors[5][j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color6:
                    fout.write((char*)&(m->mColors[6][j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color7:
                    fout.write((char*)&(m->mColors[7][j][ind]), sizeof(ai_real));
                    break;
                default:
                    break;
                }
            }
        }
    }

    fout.close();
    std::cout << "mesh compilation succeded\n";
    return 0;
}
