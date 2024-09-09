#include "meshCompiler.h"
#include <iostream>
#include <sstream>
#include <map>
#include <assimpReader.h>

// ========== DEFINES AND MAPS ==========
#define mc_version "v1.1.1"

std::map<std::string, mesh_compiler::value> mesh_compiler::preambleMap = {
    {"buffc", mc_buffers_per_unit },
    {"buffs", mc_buffer_size },
    {"entrya", mc_entries_per_unit },
    {"entryc", mc_entries_per_buffer },
    {"entrys", mc_entry_size },
    {"fielda", mc_fields_per_unit },
    {"fielde", mc_fields_per_buffer },
    {"fieldc", mc_fields_per_entry },
    {"fields", mc_field_size },
};

std::map<std::string, mesh_compiler::value> mesh_compiler::fieldsMap = {
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

std::map<std::string, mesh_compiler::type> mesh_compiler::typesMap = {
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
    {mc_long_long, sizeof(long long)},
    {mc_unsigned_short, sizeof(unsigned short)},
    {mc_unsigned_int, sizeof(unsigned int)},
    {mc_unsigned_long, sizeof(unsigned long)},
    {mc_unsigned_long_long, sizeof(unsigned long long)},
    {mc_float, sizeof(float)},
    {mc_double, sizeof(double)},
    {mc_long_double, sizeof(long double)}
};

std::map<char, unsigned short> suffixesMap = {
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
    {'x', 0}, {'y', 1}, {'z', 2},
    {'r', 0}, {'g', 1}, {'b', 2}, {'a', 3},
    {'u', 0}, {'v', 1}, {'w', 2}
};

mesh_compiler::type mesh_compiler::getDefaultValueType(const value& v)
{
    switch (v)
    {
    case mc_indice:
        return mc_unsigned_int;

    case mc_vertex:
    case mc_normal:
    case mc_tangent:
    case mc_bitangent:
    case mc_uv:
    case mc_vertex_color:
        return mc_float;

    case mc_unit_size:
    case mc_buffer_size:
    case mc_buffers_per_unit:
    case mc_entry_size:
    case mc_entries_per_unit:
    case mc_entries_per_buffer:
    case mc_field_size:
    case mc_fields_per_unit:
    case mc_fields_per_entry:
    case mc_fields_per_buffer:
        return mc_unsigned_int;

    default:
        throw std::logic_error("value type without default type");
        break;
    }
}

mesh_compiler::counting_type mesh_compiler::getFieldCount(const value& t) {
    switch (t)
    {
    case mc_vertex:
    case mc_normal:
    case mc_tangent:
    case mc_bitangent:
    case mc_uv:
    case mc_vertex_color:
        return mc_per_vertex;
    case mc_indice:
        return mc_per_indice;
    case mc_constant:
        return mc_any;
    default:
        throw std::logic_error("value with no field count querried for field count");
        break;
    }
}

std::vector<unsigned short> mesh_compiler::getMaxSuffixes(const value& t)
{
    std::vector<unsigned short> out;
    switch (t)
    {
    case mc_indice:
    case mc_vertex:
    case mc_normal:
    case mc_tangent:
    case mc_bitangent:
        out.push_back(3);
        return out;
    case mc_uv:
        out.push_back(8);
        out.push_back(3);
        return out;
    case mc_vertex_color:
        out.push_back(8);
        out.push_back(4);
        return out;
    default:
        throw std::logic_error("value with no allowed suffixes querried for max suffixes");
        break;
    }
}

void mesh_compiler::copyConstantToMemory(void* dst, const type& t, const std::string& val) {
    union data_union {
        char c;
        short s;
        int i;
        long l;
        long long ll;
        float f;
        double d;
        long double ld;
    };

    data_union data;

    switch (t)
    {
    case mc_char:
        if (val.size() > 1)
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value, "char value must be only one character, given: " + std::to_string(val.size()));
        data.c = val[0];
        break;
    case mc_short:
    case mc_unsigned_short:
        try {
            data.s = std::stoi(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        }
        break;
    case mc_int:
    case mc_unsigned_int:
        try {
            data.i = std::stoi(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        }
        break;
    case mc_long:
    case mc_unsigned_long:
        try {
            data.l = std::stol(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        }
        break;
    case mc_long_long:
    case mc_unsigned_long_long:
        try {
            data.ll = std::stoll(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        }
        break;
    case mc_float:
        try {
            data.f = std::stof(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        }
        break;
    case mc_double:
        try {
            data.d = std::stod(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        }
        break;
    case mc_long_double:
        try {
            data.ld = std::stold(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        }
        break;
    default:
        throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value);
        break;
    }

    switch (t) {
    case mc_unsigned_short:
        if (data.s < 0) throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_int:
        if (data.i < 0) throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_long:
        if (data.l < 0) throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_long_long:
        if (data.ll < 0) throw formatInterpreterException(formatInterpreterException::mc_err_invalid_const_value, "negative value passed as unsigned type");
        break;
    default:
        break;
    }
    memcpy(dst, &data, typeSizesMap[t]);
}

// ========== EXCEPTIONS ==========

std::map<int, std::string> mesh_compiler::formatInterpreterException::errorMessagesMap = {
    {formatInterpreterException::mc_err_cannot_open_file, "could not open file"},
    {formatInterpreterException::mc_err_unknown_statement, "unknown statement"},
    {formatInterpreterException::mc_err_no_suffix, "field suffix not provided"},
    {formatInterpreterException::mc_err_invalid_suffix, "invalid field suffix"},
    {formatInterpreterException::mc_err_wrong_suffixes_amount, "wrong amount of suffixes"},
    {formatInterpreterException::mc_err_no_const_value, "constant value not provided"},
    {formatInterpreterException::mc_err_invalid_const_value, "invalid const value"},
    {formatInterpreterException::mc_err_invalid_type_specifier, "invalid type specifier"},
    {formatInterpreterException::mc_err_byte_base_in_count_type, "given byte base in count type"},
    {formatInterpreterException::mc_err_field_spec_in_preamble, "attempted field specification in preamble"},
    {formatInterpreterException::mc_err_unsupported_type, "unsupported type"},
    {formatInterpreterException::mc_err_conflicting_fields, "confilcting types detected in single buffer"},
    {formatInterpreterException::mc_err_constants_only, "detected buffer of only constants - unknown buffer size"},
    {formatInterpreterException::mc_err_unknown, "unknown error"}
};

mesh_compiler::formatInterpreterException::formatInterpreterException(const error_code& error_code, const std::string& message) : type(error_code), msg(" " + message)
{
    if (errorMessagesMap.find(type) == errorMessagesMap.end()) type = formatInterpreterException::mc_err_unknown;
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

mesh_compiler::compileField::compileField(const type& s, const value& v, const void* data_source) : compileField(s, v, data_source, typeSizesMap[s])
{
}

mesh_compiler::compileField::compileField(const type& s, const value& v, const void* data_source, const size_t& data_amount) : stype(s), vtype(v)
{
    if (s == mc_none) throw std::logic_error("unable to construct compile Field of type: none");
    if (v == mc_null) throw std::logic_error("unable to construct compile Field of value: null");
    if (data_source != nullptr) {
        setData(data_source, data_amount);
    }
}

void mesh_compiler::compileField::setAsConst(const type& t, const void* data_source)
{
    vtype = mc_constant;
    setData(data_source, typeSizesMap[t]);
}

void mesh_compiler::compileField::setData(const void* data_source, const size_t& data_amount)
{
    data.resize(data_amount);
    memcpy(data.data(), data_source, data_amount);
}

size_t mesh_compiler::compileField::get_size() const
{
    return typeSizesMap[stype];
}

void mesh_compiler::compileField::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "field info: type: " << stype << ":" << vtype;
    for (const char& c : data) std::cout << c;
}

size_t mesh_compiler::compileBuffer::get_entry_size() const
{
    size_t siz = 0;
    for (const compileField& cf : fields)
        siz += cf.get_size();
    return siz;
}

size_t mesh_compiler::compileBuffer::get_size() const
{
    return get_entry_size() * count;
}

void mesh_compiler::compileBuffer::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "buffer info: preamble: ";
    for (const compileField& f : preamble) {
        f.print();
    }
    std::cout << ", count: " << count << ", fields: \n";
    for (const compileField& f : fields) {
        f.print(indent + 1);
        std::cout << std::endl;
    }
}

void mesh_compiler::compileBuffer::clear()
{
    this->preamble.clear();
    this->fields.clear();
}

size_t mesh_compiler::compileUnit::get_size()
{
    size_t siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.get_size();
    return siz;
}

size_t mesh_compiler::compileUnit::get_entries_count()
{
    size_t siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.count;
    return siz;
}

size_t mesh_compiler::compileUnit::get_fields_count()
{
    size_t siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.fields.size();
    return siz;
}

void mesh_compiler::compileUnit::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "config info:\n";
    std::cout << " preamble info: ";
    for (const compileField& f : preamble) f.print();
    std::cout << "\n";
    for (const compileBuffer& b : buffers) b.print(indent + 1);
}

void mesh_compiler::compileUnit::clear()
{
    this->preamble.clear();
    this->buffers.clear();
}

mesh_compiler::type mesh_compiler::compileUnit::extractType(std::string& word)
{
    type t = mc_none;
    size_t pos = word.find(':');
    if (pos != std::string::npos) {
        if (typesMap.find(word.substr(0, pos)) == typesMap.end()) throw formatInterpreterException(formatInterpreterException::mc_err_invalid_type_specifier);
        t = typesMap[word.substr(0, pos)];
        word = word.substr(pos + 1, word.size() - pos - 1);
    }

    if (word.empty()) {
        if (t != mc_none) throw formatInterpreterException(formatInterpreterException::mc_err_no_const_value);
        else throw std::logic_error("type somehow ended up null despite empty word");
    }

    return t;
}

mesh_compiler::value mesh_compiler::compileUnit::extractPreambleValue(std::string& word)
{
    value v = mc_null;
    if (preambleMap.find(word) != preambleMap.end()) {
        v = preambleMap[word];
        word = "";
    }
    return v;
}

mesh_compiler::value mesh_compiler::compileUnit::extractFieldValue(std::string& word)
{
    value v = mc_null;
    size_t pos = word.find('.');
    std::string ftype = word;
    if (pos != std::string::npos) {
        ftype = word.substr(0, pos);
    }
    else pos = word.size();
    if (fieldsMap.find(ftype) != fieldsMap.end()) {
        v = fieldsMap[ftype];
        word = word.substr(pos, word.size() - pos);
    }
    return v;
}

bool mesh_compiler::compileUnit::isPreambleValue(type t, std::string& arg, std::vector<compileField>& fields)
{
    value v = extractPreambleValue(arg);
    if (v != mc_null) {
        if (t == mc_none) t = getDefaultValueType(v);
        fields.push_back(compileField(t, v, nullptr, 0));
        return true;
    }
    return false;
}

bool mesh_compiler::compileUnit::isFieldValue(type t, std::string& arg, std::vector<compileField>& fields, counting_type& field_count)
{
    value v = extractFieldValue(arg);
    if (v != mc_null) {
        if (t == mc_none) t = getDefaultValueType(v);
        compileField field(t, v, nullptr, 0);

        // counting type
        counting_type ffc = getFieldCount(field.vtype);
        if (ffc != mc_any) {
            if (ffc != field_count && field_count != mc_any) {
                std::string error_message = " conflicting types: ";
                error_message.push_back(field.vtype);
                error_message += " and ";
                error_message.push_back(field_count);
                error_message += ".";
                throw formatInterpreterException(formatInterpreterException::mc_err_conflicting_fields, error_message);
            }
            field_count = ffc;
        }

        std::vector<unsigned short> suffixes_data = getMaxSuffixes(v);
        while (arg.size() > 0) {
            if (arg.size() == 1) {
                if (arg[0] == '.') throw formatInterpreterException(formatInterpreterException::mc_err_no_suffix);
                else throw formatInterpreterException(formatInterpreterException::mc_err_unknown_statement);
            }
            if (arg[0] != '.') throw formatInterpreterException(formatInterpreterException::mc_err_unknown_statement);
            if (suffixesMap.find(arg[1]) == suffixesMap.end()) throw formatInterpreterException(formatInterpreterException::mc_err_invalid_suffix);

            field.data.push_back(suffixesMap[arg[1]]);
            if (field.data.size() > suffixes_data.size()) throw formatInterpreterException(formatInterpreterException::mc_err_wrong_suffixes_amount, "this field type requires up to " + std::to_string(suffixes_data.size()) + " suffixes");
            if (field.data.back() >= suffixes_data[field.data.size() - 1]) throw formatInterpreterException(formatInterpreterException::mc_err_invalid_suffix, arg.substr(0, 2) + " suffix must be less than " + std::to_string(suffixes_data[field.data.size() - 1]));

            arg = arg.substr(2, arg.size() - 2);
        }

        if (field.data.size() == suffixes_data.size()) {
            fields.push_back(field);
            return true;
        }

        for (int j = field.data.size(); j < suffixes_data.size(); ++j) {
            field.data.push_back(0);
            for (int i = 0; i < suffixes_data[j]; ++i) {
                field.data.back() = i;
                fields.push_back(field);
            }
        }

        return true;
    }
    return false;
}

bool mesh_compiler::compileUnit::isConstValue(const type& t, std::string& arg, std::vector<compileField>& fields)
{
    if (t == mc_none) return false;

    fields.push_back(compileField(t, mc_constant, nullptr, 0));
    fields.back().data.resize(typeSizesMap[t]);
    copyConstantToMemory(fields.back().data.data(), t, arg);
    arg = "";

    return true;
}

mesh_compiler::compileUnit::compileUnit(const std::string& filename)
{
    std::ifstream formatFile(filename, std::ios::in);
    if (!formatFile) {
        throw formatInterpreterException(formatInterpreterException::mc_err_cannot_open_file, filename);
    }

    std::string line, arg = "";

    // preamble
    std::getline(formatFile, line);
    line += ' ';
    for (const char& c : line) {
        if ((c >= 9 && c <= 13) || c == ' ') {
            if (arg.empty()) continue;
            else { // process word
                std::string word = arg;
                try {
                    type t = extractType(arg);

                    if (isPreambleValue(t, arg, this->preamble)) continue;

                    if (isConstValue(t, arg, this->preamble)) continue;
                    
                    throw formatInterpreterException(formatInterpreterException::mc_err_unknown_statement);
                }
                catch (formatInterpreterException& e){
                    e.fillInfo(1, word);
                    throw;
                }
                arg = "";
            }
        }
        else arg += c;
    }

    // buffers
    int line_num = 2;
    while (std::getline(formatFile, line)) {
        line += ' ';
        compileBuffer buffer;
        counting_type field_count = mc_any;
        bool fields_def = false;
        for (const char& c : line) {
            if ((c >= 9 && c <= 13) || c == ' ') { // whitespace character
                if (arg.empty()) continue;
                else { // process word
                    std::string word = arg;

                    if (!fields_def) { // preamble

                        if (arg.size() == 1 && arg[0] == ';') {
                            fields_def = true;
                            arg = "";
                            continue;
                        }
                        try {
                            type t = extractType(arg);

                            if (isPreambleValue(t, arg, buffer.preamble)) continue;

                            if (isFieldValue(t, arg, buffer.fields, field_count)) {
                                fields_def = true;
                                continue;
                            }

                            if (isConstValue(t, arg, buffer.preamble)) continue;

                            throw formatInterpreterException(formatInterpreterException::mc_err_unknown_statement);
                        }
                        catch (formatInterpreterException& e) {
                            e.fillInfo(line_num, word);
                            throw;
                        }
                    }

                    else { // fields

                        try {
                            type t = extractType(arg);

                            if (isFieldValue(t, arg, buffer.fields, field_count)) continue;

                            if (isConstValue(t, arg, buffer.fields)) continue;

                            throw formatInterpreterException(formatInterpreterException::mc_err_unknown_statement);
                        }
                        catch (formatInterpreterException& e) {
                            e.fillInfo(line_num, word);
                            throw;
                        }
                    }
                }
            }
            else arg += c;
        }
        if (field_count == mc_any) {
            formatInterpreterException e(formatInterpreterException::mc_err_constants_only);
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
    catch (std::runtime_error& e) {
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

void mesh_compiler::compileMesh(const aiMesh* m, compilationInfo ci)
{
    // fill counts
    for (compileBuffer& buffer : ci.config.buffers) {
        counting_type field_count = mc_any;
        for (const compileField& cf : buffer.fields) {
            counting_type ffc = getFieldCount(cf.vtype);
            if (ffc != mc_any) {
                if (ffc != field_count && field_count != mc_any) {
                    throw meshCompilerException("format error: confilcting types detected in single buffer: conflicting types: " + std::to_string(cf.vtype) + " and " + std::to_string(ffc));
                }
                field_count = ffc;
            }
        }
        if (field_count == mc_any) {
            throw meshCompilerException("format error: detected buffer of only constants: unknown buffer size");
        }
        else if (field_count == mc_per_indice) buffer.count = m->mNumFaces;
        else if (field_count == mc_per_vertex) buffer.count = m->mNumVertices;
        else {
            throw meshCompilerException("format error: unknown field type count");
        }
    }

    // output file creation
    std::ofstream fout(ci.output_file, std::ios::out | std::ios::binary);
    if (!fout) {
        throw std::runtime_error("compilation error: cannot open file: " + ci.output_file);
    }

    // preamble
    for (const compileField& field : ci.config.preamble) {
        // size
        switch (field.vtype)
        {
        case mc_constant:
            fout.write(field.data.data(), typeSizesMap[field.stype]);
            break;
        case mc_buffer_size:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.get_size(), field.stype);
            }
            break;
        case mc_buffers_per_unit:
            writeConst(fout, ci.config.buffers.size(), field.stype);
            break;
        case mc_entry_size:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.get_entry_size(), field.stype);
            }
            break;
        case mc_entries_per_buffer:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.count, field.stype);
            }
            break;
        case mc_entries_per_unit:
            writeConst(fout, ci.config.get_entries_count(), field.stype);
            break;
        case mc_field_size:
            for (const compileBuffer& cb : ci.config.buffers) {
                for (const compileField& cf : cb.fields) {
                    writeConst(fout, cf.get_size(), field.stype);
                }
            }
            break;
        case mc_fields_per_entry:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.fields.size(), field.stype);
            }
            break;
        case mc_fields_per_buffer:
            for (const compileBuffer& cb : ci.config.buffers) {
                writeConst(fout, cb.fields.size() * cb.count, field.stype);
            }
            break;
        case mc_fields_per_unit:
            writeConst(fout, ci.config.get_fields_count(), field.stype);
            break;
        default:
            throw meshCompilerException("compilation error: unknown buffer info flag: " + std::to_string(field.vtype));
            break;
        }
    }

    // buffers
    for (const compileBuffer& cb : ci.config.buffers) {

        // format info
        for (const compileField& field : cb.preamble) {

            // size
            switch (field.vtype)
            {
            case mc_constant:
                fout.write(field.data.data(), typeSizesMap[field.stype]);
                break;
            case mc_buffer_size:
                writeConst(fout, cb.get_size(), field.stype);
                break;
            case mc_entry_size:
                writeConst(fout, cb.get_entry_size(), field.stype);
                break;
            case mc_entries_per_buffer:
                writeConst(fout, cb.count, field.stype);
                break;
            case mc_field_size:
                for (const compileField& cf : cb.fields) {
                    writeConst(fout, cf.get_size(), field.stype);
                }
                break;
            case mc_fields_per_entry:
                writeConst(fout, cb.fields.size(), field.stype);
                break;
            case mc_fields_per_buffer:
                writeConst(fout, cb.fields.size()* cb.count, field.stype);
                break;
            default:
                throw meshCompilerException("compilation error: unknown buffer info flag: " + std::to_string(field.vtype));
                break;
            }
        }

        // data buffers
        for (unsigned int j = 0; j < cb.count; ++j) {
            for (const compileField& field : cb.fields) {
                //unsigned short ind = field.data[0];
                switch (field.vtype)
                {
                case mc_constant:
                    fout.write(field.data.data(), typeSizesMap[field.stype]);
                    break;
                case mc_indice:
                    writeConst(fout, m->mFaces[j].mIndices[field.data[0]], field.stype);
                    break;
                case mc_vertex:
                    writeConst(fout, m->mVertices[j][field.data[0]], field.stype);
                    break;
                case mc_normal:
                    writeConst(fout, m->mNormals[j][field.data[0]], field.stype);
                    break;
                case mc_uv:
                    writeConst(fout, m->mTextureCoords[field.data[0]][j][field.data[1]], field.stype);
                    break;
                case mc_tangent:
                    writeConst(fout, m->mTangents[j][field.data[0]], field.stype);
                    break;
                case mc_bitangent:
                    writeConst(fout, m->mBitangents[j][field.data[0]], field.stype);
                    break;
                case mc_vertex_color:
                    writeConst(fout, m->mColors[field.data[0]][j][field.data[1]], field.stype);
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
