#include "meshCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <assimpReader.h>

// ========== DEFINES AND MAPS ==========
#define mc_version "v1.1.1"

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

std::map<std::string, mesh_compiler::type> mesh_compiler::fieldsMap = {
    { "i", mc_indice},
    { "indice", mc_indice},
    { "v", mc_vertex},
    { "vertex", mc_vertex},
    { "n", mc_normal},
    { "normal", mc_normal},
    { "tc", mc_uv},
    { "tex_coord", mc_uv},
    { "texture_coordinate", mc_uv},
    { "uv", mc_uv },
    { "t", mc_tangent },
    { "tangent", mc_tangent },
    { "b", mc_bitangent },
    { "bitangent", mc_bitangent},
    { "vertex_color0", mc_vertex_color }
};

std::map<std::string, mesh_compiler::type> mesh_compiler::constsMap = {
    {"char", mc_char},
    {"short", mc_short},
    {"int", mc_int},
    {"long", mc_long},
    {"long_long", mc_long_long},
    {"int2", mc_short},
    {"int4", mc_int},
    {"int8", mc_long},
    {"int16", mc_long_long},
    {"unsigned_short", mc_unsigned_short},
    {"unsigned_int", mc_unsigned_int},
    {"unsigned_long", mc_unsigned_long},
    {"unsigned_int2", mc_unsigned_short},
    {"unsigned_int4", mc_unsigned_int},
    {"unsigned_int8", mc_unsigned_long},
    {"unsigned_int16", mc_unsigned_long_long},
    {"ushort", mc_unsigned_short},
    {"uint", mc_unsigned_int},
    {"ulong", mc_unsigned_long},
    {"uint2", mc_unsigned_short},
    {"uint4", mc_unsigned_int},
    {"uint8", mc_unsigned_long},
    {"uint16", mc_unsigned_long_long},
    {"float", mc_float},
    {"float4", mc_float},
    {"double", mc_double},
    {"float8", mc_double},
    {"long_double", mc_long_double},
    {"float16", mc_long_double}
};

std::map<mesh_compiler::type, unsigned short> mesh_compiler::typeSizesMap = {
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

char mesh_compiler::getFieldCount(const type& t) {
    switch (t)
    {
    case mc_vertex:
    case mc_normal:
    case mc_tangent:
    case mc_bitangent:
    case mc_uv:
    case mc_vertex_color:
        return 'v';
    case mc_indice:
        return 'i';
    default:
        if (typeSizesMap.find(t) != typeSizesMap.end()) return 0;
        return 128;
        break;
    }
}

std::map<char, unsigned short> suffixesMap = {
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3},
    {'x', 0}, {'y', 1}, {'z', 2},
    {'r', 0}, {'g', 1}, {'b', 2}, {'a', 3},
    {'u', 0}, {'v', 1}, {'w', 2}
};

// ========== EXCEPTIONS ==========

// error defines
#define mc_err_cannot_open_file 1
#define mc_err_unknown_statement 2
#define mc_err_no_suffix 3
#define mc_err_invalid_suffix 4
#define mc_err_no_const_value 5
#define mc_err_invalid_const_value 6
#define mc_err_byte_base_in_count_type 7
#define mc_err_field_spec_in_preamble 8
#define mc_err_unsupported_type 9
#define mc_err_conflicting_fields 10
#define mc_err_constants_only 11
#define mc_err_unknown 100

std::map<int, std::string> mesh_compiler::formatInterpreterException::errorMessagesMap = {
    {mc_err_cannot_open_file, "could not open file"},
    {mc_err_unknown_statement, "unknown statement"},
    {mc_err_no_suffix, "field suffix not provided"},
    {mc_err_invalid_suffix, "invalid field suffix"},
    {mc_err_no_const_value, "constant value not provided"},
    {mc_err_invalid_const_value, "invalid const value"},
    {mc_err_byte_base_in_count_type, "given byte base in count type"},
    {mc_err_field_spec_in_preamble, "attempted field specification in preamble"},
    {mc_err_unsupported_type, "unsupported type"},
    {mc_err_conflicting_fields, "confilcting types detected in single buffer"},
    {mc_err_constants_only, "detected buffer of only constants - unknown buffer size"},
    {mc_err_unknown, "unknown error"}
};

mesh_compiler::formatInterpreterException::formatInterpreterException(const int& error_code, const std::string& message) : type(error_code), msg(" " + message)
{
    if (errorMessagesMap.find(type) == errorMessagesMap.end()) type = mc_err_unknown;
}

void mesh_compiler::formatInterpreterException::fillInfo(const unsigned int& line_number, const std::string& processed_word)
{
    this->msg = "format compilation error: " + errorMessagesMap[type] + ": " + processed_word + " in line " + std::to_string(line_number) + "." + this->msg;
}

const char* mesh_compiler::formatInterpreterException::what() throw()
{
    if (msg.size() < 10) this->msg = "format compilation error: " + errorMessagesMap[type];
    return this->msg.c_str();
}

mesh_compiler::meshCompilerException::meshCompilerException(const std::string& message) : msg(message)
{
}

const char* mesh_compiler::meshCompilerException::what() throw()
{
    return msg.c_str();
}

// ========== METHODS DEFINITIONS ==========

unsigned int mesh_compiler::compileField::get_size(unsigned short byte_base) const
{
    return 4 / byte_base;
}

void mesh_compiler::compileField::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    printf("field info: type: %c, data: ", type);
    for (const char& c : data) std::cout << c;
    std::cout << std::endl;
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

bool mesh_compiler::compileConfig::isArgument(const std::string& arg, compilePreamble& preamble)
{
    if (arg.size() >= 4 && argsMap.find(arg.substr(0, arg.size() - 3)) != argsMap.end()) { // argument
        if (arg[arg.size() - 2] != '_') {
            std::string correct = arg;
            correct[correct.size() - 2] = '_';
            throw formatInterpreterException(mc_err_unknown_statement, "did you mean: " + correct + "?");
        }
        if (arg[arg.size() - 4] != 's') {
            throw formatInterpreterException(mc_err_byte_base_in_count_type);
        }
        char varg = argsMap[arg.substr(0, arg.size() - 3)];
        char size = arg[arg.size() - 3];
        char base = arg[arg.size() - 1];
        if (sizeMap.find(size) == sizeMap.end() || baseMap.find(base) == baseMap.end()) {
            throw formatInterpreterException(mc_err_unknown_statement);
        }
        size = sizeMap[size];
        base = baseMap[base];
        preamble.info_format.push_back(varg | size | base);
        return true;
    }
    else if (arg.size() >= 2 && argsMap.find(arg.substr(0, arg.size() - 1)) != argsMap.end()) {
        char varg = argsMap[arg.substr(0, arg.size() - 1)];
        char size = arg[arg.size() - 1];
        if (sizeMap.find(size) == sizeMap.end()) {
            throw formatInterpreterException(mc_err_unknown_statement);
        }
        size = sizeMap[size];
        preamble.info_format.push_back(varg | size | mc_x_4);
        return true;
    }
    else if (argsMap.find(arg) != argsMap.end()) {
        char varg = argsMap[arg];
        preamble.info_format.push_back(varg | mc_4_4);
        return true;
    }
    return false;
}

bool mesh_compiler::compileConfig::isField(const std::string& arg, std::vector<compileField>& fields, char& field_count)
{
    size_t pos = arg.find('.');
    if (pos != arg.size() - 2) {
        return false;
    }
    std::string type = arg.substr(0, pos);
    char suffix = arg[pos+1];
    if (fieldsMap.find(type) != fieldsMap.end()) { // field
        if (suffixesMap.find(suffix) != suffixesMap.end()) {
            compileField field;
            field.type = fieldsMap[type];
            char ffc = getFieldCount(field.type);
            if (ffc != 0) {
                if (ffc != field_count && field_count != 0) {
                    std::string error_message = " conflicting types: ";
                    error_message.push_back(field.type);
                    error_message += " and ";
                    error_message.push_back(field_count);
                    error_message += ".";
                    throw formatInterpreterException(mc_err_conflicting_fields, error_message);
                }
                field_count = ffc;
            }
            field.data.resize(sizeof(unsigned short));
            memcpy(field.data.data(), &suffixesMap[suffix], sizeof(unsigned short));
            fields.push_back(field);
            return true;
        }
        else {
            throw formatInterpreterException(mc_err_invalid_suffix);
        }
    }
    else if (fieldsMap.find(arg) != fieldsMap.end()) { // field
        throw formatInterpreterException(mc_err_no_suffix);
    }
    return false;
}

void mesh_compiler::copyConstantToMemory(void* dst, const type& t, const std::string& val) {
    union data_union {
        char c;
        short s;
        int i;
        long l;
        float f;
        double d;
        long double ld;
    };
    
    data_union data;

    switch (t)
    {
    case mc_char:
        if (val.size() > 1)
            throw formatInterpreterException(mc_err_invalid_const_value, "char value must be only one character, given: " + std::to_string(val.size()));
        data.c = val[0];
        break;
    case mc_short:
    case mc_unsigned_short:
        try {
            data.s = std::stoi(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(mc_err_invalid_const_value);
        }
        break;
    case mc_int:
    case mc_unsigned_int:
        try {
            data.i = std::stoi(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(mc_err_invalid_const_value);
        }
        break;
    case mc_long:
    case mc_unsigned_long:
        try {
            data.l = std::stol(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(mc_err_invalid_const_value);
        }
        break;
    case mc_float:
        try {
            data.f = std::stof(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(mc_err_invalid_const_value);
        }
        break;
    case mc_double:
        try {
            data.d = std::stod(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(mc_err_invalid_const_value);
        }
        break;
    case mc_long_double:
        try {
            data.ld = std::stold(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(mc_err_invalid_const_value);
        }
        break;
    default:
        throw formatInterpreterException(mc_err_invalid_const_value);
        break;
    }

    switch (t) {
    case mc_unsigned_short:
        if (data.s < 0) throw formatInterpreterException(mc_err_invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_int:
        if (data.i < 0) throw formatInterpreterException(mc_err_invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_long:
        if (data.l < 0) throw formatInterpreterException(mc_err_invalid_const_value, "negative value passed as unsigned type");
        break;
    default:
        break;
    }
    memcpy(dst, &data, typeSizesMap[t]);
}

bool mesh_compiler::compileConfig::isType(const std::string& arg, compilePreamble& preamble)
{
    size_t pos = arg.find(':');
    if (pos == std::string::npos) {
        if (constsMap.find(arg) != constsMap.end()) {
            throw formatInterpreterException(mc_err_no_const_value);
        }
        return false;
    }
    if (pos + 1 == arg.size())
        throw formatInterpreterException(mc_err_no_const_value);

    std::string t = arg.substr(0, pos);
    if (constsMap.find(t) != constsMap.end()) { // constant
        type ctype = constsMap[t];
        int siz = typeSizesMap[ctype];
        preamble.info_format.push_back((char)siz);

        preamble.data.resize(preamble.data.size() + siz);

        copyConstantToMemory(&(preamble.data[preamble.data.size() - siz]), ctype, arg.substr(pos + 1, arg.size() - pos - 1));

        return true;
    }
    return false;
}

bool mesh_compiler::compileConfig::isType(const std::string& arg, std::vector<compileField>& fields)
{
    size_t pos = arg.find(':');
    if (pos == std::string::npos) {
        if (constsMap.find(arg) != constsMap.end()) {
            throw formatInterpreterException(mc_err_no_const_value);
        }
        return false;
    }
    if (pos+1 == arg.size())
        throw formatInterpreterException(mc_err_no_const_value);

    std::string type = arg.substr(0, pos);
    if (constsMap.find(type) != constsMap.end()) { // constant
        compileField field;
        field.type = constsMap[type];
        field.data.resize(typeSizesMap[field.type]);
        copyConstantToMemory(field.data.data(), field.type, arg.substr(pos + 1, arg.size() - pos - 1));
        fields.push_back(field);
        return true;
    }
    return false;
}

mesh_compiler::compileConfig::compileConfig(const std::string& filename)
{
    std::ifstream formatFile(filename, std::ios::in);
    if (!formatFile) {
        ;
        throw formatInterpreterException(mc_err_cannot_open_file, filename);
    }

    std::string line, arg = "";

    // preamble
    std::getline(formatFile, line);
    line += ' ';
    for (const char& c : line) {
        if ((c >= 9 && c <= 13) || c == ' ') {
            if (arg.empty()) continue;
            else { // process word
                
                try {
                    if (isArgument(arg, this->preamble)) {
                        arg = "";
                        continue;
                    }
                    else if (isType(arg, this->preamble)) {
                        arg = "";
                        continue;
                    }
                    else {
                        throw formatInterpreterException(mc_err_unknown_statement);
                    }
                }
                catch (formatInterpreterException& e){
                    e.fillInfo(1, arg);
                    throw;
                }
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
        bool fields_def = false;
        for (const char& c : line) {
            if ((c >= 9 && c <= 13) || c == ' ') { // whitespace character
                if (arg.empty()) continue;
                else { // process word

                    if (!fields_def) { // preamble

                        if (arg.size() == 1 && arg[0] == ';') {
                            fields_def = true;
                            arg = "";
                            continue;
                        }
                        try {
                            if (isArgument(arg, buffer.preamble)) {
                                arg = "";
                                continue;
                            }
                            else if (isField(arg, buffer.fields, field_count)) {
                                fields_def = true;
                                arg = "";
                                continue;
                            }
                            else if (isType(arg, buffer.preamble)) {
                                arg = "";
                                continue;
                            }
                            else {
                                throw formatInterpreterException(mc_err_unknown_statement);
                            }
                        }
                        catch (formatInterpreterException& e) {
                            e.fillInfo(line_num, arg);
                            throw;
                        }
                    }

                    else { // fields

                        try {
                            if (isField(arg, buffer.fields, field_count)) {
                                arg = "";
                                continue;
                            }
                            else if (isType(arg, buffer.fields)) {
                                arg = "";
                                continue;
                            }
                            else {
                                throw formatInterpreterException(mc_err_unknown_statement);
                            }
                        }
                        catch (formatInterpreterException& e) {
                            e.fillInfo(line_num, arg);
                            throw;
                        }
                    }
                }
            }
            else arg += c;
        }
        if (field_count == 0) {
            formatInterpreterException e(mc_err_constants_only);
            e.fillInfo(line_num, "");
            throw e;
        }
        this->buffers.push_back(buffer);
        ++line_num;
    }

    formatFile.close();
    std::cout << "format file compilation succeded\n";
}

mesh_compiler::compilationInfo::compilationInfo(const std::string& format_file, const std::string& output_file, const bool& debug_messages) : output_file(output_file), config(format_file), debug_messages(true)
{
    if (this->debug_messages) this->config.print();
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
                std::string a;
                ss >> a;
                if (!a.empty()) args.push_back(a);
            }
            runOnce(args);
        }
    }
    else {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        runOnce(args);
    }
}

void mesh_compiler::runOnce(const std::vector<std::string>& args)
{
    if (args.size() == 1 && (args[0] == "-v" || args[0] == "--version")) {
        std::cout << mc_version << std::endl;
        return;
    }
    try {
        compile(args);
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

void mesh_compiler::compile(const std::vector<std::string>& args)
{
    int siz = args.size();
    if (siz == 0) {
        throw std::runtime_error("source file not specified");
    }

    std::string format_file = ".format";
    std::string output_file = "{file}_{mesh}.mesh";
    bool debug_messages = false;

    for (int i = 1; i < siz; ++i) {
        if (args[i] == "-f") {
            ++i;
            if (i == siz) {
                throw std::runtime_error("error: unspecified format file: -f <format file path>");
            }
            format_file = args[i];
        }
        else if (args[i] == "-o") {
            ++i;
            if (i == siz) {
                throw std::runtime_error("error: unspecified output file: -o <output file path>");
            }
            output_file = args[i];
        }
        else if (args[i] == "-d") {
            if (debug_messages) {
                throw std::runtime_error("error: -d flag encountered more than once");
            }
            debug_messages = true;
        }
        else if (i == 1) format_file = args[i];
        else if (i == 2 && !debug_messages) output_file = args[i];
    }
    try {
        try {
            compileFile(args[0], compilationInfo(format_file, output_file, debug_messages));
        }
        catch (formatInterpreterException& e) {
            std::cout << e.what() << std::endl;
            throw meshCompilerException("format file compilation ended with errors could not compile file");
        }
    }
    catch (meshCompilerException& e) {
        std::cout << e.what() << std::endl;
    }
}

void mesh_compiler::compileFile(const std::string& filename, compilationInfo ci)
{
    // replace {file} with file name in output file name
    size_t found = ci.output_file.find("{file}");
    if (found != std::string::npos) {
        std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
        size_t const p(base_filename.find_last_of('.'));
        ci.output_file.replace(found, 6, base_filename.substr(0, p));
    }

    assimp::readFile(filename, std::bind(mesh_compiler::compileScene, std::placeholders::_1, std::ref(ci)));
}

void mesh_compiler::compileScene(const aiScene* scene, compilationInfo ci)
{
    // replace {scene} with scene name in output file name
    size_t found = ci.output_file.find("{scene}");
    if (found != std::string::npos) ci.output_file.replace(found, 7, scene->mName.C_Str());

    // find name and extension
    std::string orig_name = ci.output_file;
    int errors = 0;
    for (int i = 0; i < scene->mNumMeshes; ++i) {
        size_t found = ci.output_file.find("{mesh}");
        if (found != std::string::npos) ci.output_file.replace(found, 6, scene->mMeshes[i]->mName.C_Str());
        try {
            compileMesh(scene->mMeshes[i], ci);
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            std::cout << "compilation of mesh: " << scene->mMeshes[i]->mName.C_Str() << " ended up with errors.\n";
            errors += 1;
        }
        ci.output_file = orig_name; // go back to original name
    }

    if (errors != 0) {
        std::cout << "scene compilation ended with errors\n";
        printf("compiled %d out of %d meshes\n", scene->mNumMeshes - errors, scene->mNumMeshes);
        return;
    }
    std::cout << "scene compilation ended with success\n";
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

void mesh_compiler::compileMesh(const aiMesh* m, compilationInfo ci)
{
    // fill counts
    for (compileBuffer& buffer : ci.config.buffers) {
        char field_count = 0;
        for (const compileField& cf : buffer.fields) {
            char ffc = getFieldCount(cf.type);
            if (ffc != 0) {
                if (ffc != field_count && field_count != 0) {
                    throw meshCompilerException("format error: confilcting types detected in single buffer: conflicting types: " + std::string(1, cf.type) + " and " + std::string(1, field_count));
                }
                field_count = ffc;
            }
        }
        if (field_count == 0) {
            throw meshCompilerException("format error: detected buffer of only constants: unknown buffer size");
        }
        else if (field_count == 'i') buffer.count = m->mNumFaces;
        else if (field_count == 'v') buffer.count = m->mNumVertices;
        else {
            throw meshCompilerException("format error: unknown field type count");
        }
    }

    // output file creation
    std::ofstream fout(ci.output_file, std::ios::out | std::ios::binary);
    if (!fout) {
        throw std::runtime_error("compilation error: cannot open file: " + ci.output_file);
    }

    size_t data_pointer = 0;
    // preamble
    for (const char& c : ci.config.preamble.info_format) {

        // base
        unsigned short base = 1;
        if ((c & mc_x_mask) == mc_x_2) base = 2;
        else if ((c & mc_x_mask) == mc_x_4) base = 4;
        else if ((c & mc_x_mask) == mc_x_8) base = 8;

        // size
        switch (c & mc_mask)
        {
        case mc_constant:
            fout.write(&(ci.config.preamble.data[data_pointer]), c & mc_mask_mask);
            data_pointer += c & mc_mask_mask;
            break;
        case mc_buffer:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.get_size(base), c);
            }
            break;
        case mc_buffers_count:
            writeConst(fout, ci.config.buffers.size(), c);
            break;
        case mc_entry:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.get_entry_size(base), c);
            }
            break;
        case mc_entries_count:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.count, c);
            }
            break;
        case mc_buffers_x_entries:
            writeConst(fout, ci.config.get_entries_count(), c);
            break;
        case mc_field:
            for (const compileBuffer& cb : ci.config.buffers) {
                for (const compileField& cf : cb.fields) {
                    writeConst(fout, cf.get_size(base), c);
                }
            }
            break;
        case mc_fields_count:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.fields.size(), c);
            }
            break;
        case mc_entries_x_fields:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.fields.size() * cb.count, c);
            }
            break;
        case mc_buffers_x_entries_x_fields:
            writeConst(fout, ci.config.get_fields_count(), c);
            break;
        default:
            throw meshCompilerException("compilation error: unknown buffer info flag: " + std::string(1, c));
            break;
        }
    }

    // buffers
    for (const compileBuffer& cb : ci.config.buffers) {

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
                throw meshCompilerException("compilation error: unknown buffer info flag: " + std::string(1, c));
                break;
            }
        }

        // data buffers
        for (unsigned int j = 0; j < cb.count; ++j) {
            for (const compileField& cf : cb.fields) {
                unsigned short ind = 0;
                memcpy((char*)&ind, cf.data.data(), sizeof(unsigned short));
                switch (cf.type)
                {
                case mc_char:
                case mc_short:
                case mc_int:
                case mc_long:
                case mc_long_long:
                case mc_unsigned_short:
                case mc_unsigned_int:
                case mc_unsigned_long:
                case mc_unsigned_long_long:
                case mc_float:
                case mc_double:
                case mc_long_double:
                    fout.write(cf.data.data(), typeSizesMap[cf.type]);
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
                case mc_uv:
                    fout.write((char*)&(m->mTextureCoords[0][j][ind]), sizeof(ai_real));
                    break;
                case mc_tangent:
                    fout.write((char*)&(m->mTangents[j][ind]), sizeof(ai_real));
                    break;
                case mc_bitangent:
                    fout.write((char*)&(m->mBitangents[j][ind]), sizeof(ai_real));
                    break;
                case mc_vertex_color:
                    fout.write((char*)&(m->mColors[0][j][ind]), sizeof(ai_real));
                    break;
                default:
                    break;
                }
            }
        }
    }

    fout.close();
    std::cout << "mesh: " + std::string(m->mName.C_Str()) + " compilation succeded\n";
}
