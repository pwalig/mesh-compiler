#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include <assimp/scene.h>

class mesh_compiler {
public:
    mesh_compiler() = delete;
    mesh_compiler(const mesh_compiler& other) = delete;
    mesh_compiler(mesh_compiler&& other) = delete;

private:

// ========== TYPES ==========

    enum type {
        mc_none,
        mc_char,
        mc_short,
        mc_unsigned_short,
        mc_int,
        mc_unsigned_int,
        mc_long,
        mc_unsigned_long,
        mc_long_long,
        mc_unsigned_long_long,
        mc_float,
        mc_double, mc_long_double
    };
    
    enum value {
        mc_null,
        mc_constant,
        mc_indice,
        mc_vertex,
        mc_normal,
        mc_tangent,
        mc_bitangent,
        mc_uv,
        mc_vertex_color,
        mc_unit_size,
        mc_buffer_size,
        mc_buffers_per_unit,
        mc_entry_size,
        mc_entries_per_unit,
        mc_entries_per_buffer,
        mc_field_size,
        mc_fields_per_unit,
        mc_fields_per_buffer,
        mc_fields_per_entry
    };
    
    enum counting_type {
        mc_any,
        mc_once,
        mc_per_indice,
        mc_per_vertex,
        mc_per_bone,
        mc_per_mesh,
        mc_per_skeleton,
        mc_per_animation
    };

    static type getDefaultValueType(const value& v);
    static counting_type getFieldCount(const value& t);
    static std::vector<unsigned short> getMaxSuffixes(const value& t);
    static void copyConstantToMemory(void* dst, const type& type, const std::string& val);

    static std::map<std::string, value> preambleMap;
    static std::map<std::string, value> fieldsMap;
    static std::map<std::string, type> typesMap;
    static std::map<type, unsigned short> typeSizesMap;

// ========== EXCEPTIONS ==========

    class formatInterpreterException : public std::exception {
    public:
        enum error_code {
            mc_err_cannot_open_file,
            mc_err_unknown_statement,
            mc_err_no_suffix,
            mc_err_invalid_suffix,
            mc_err_wrong_suffixes_amount,
            mc_err_no_const_value,
            mc_err_invalid_const_value,
            mc_err_invalid_type_specifier,
            mc_err_byte_base_in_count_type,
            mc_err_field_spec_in_preamble,
            mc_err_unsupported_type,
            mc_err_conflicting_fields,
            mc_err_constants_only,
            mc_err_unknown
        };
        formatInterpreterException(const error_code& error_code, const std::string& message = "");
        void fillInfo(const unsigned int& line_number, const std::string& processed_word);
        virtual const char* what() throw();
    protected:
        error_code type;
        std::string msg = "";
    private:
        static std::map<int, std::string> errorMessagesMap;
    };

    class meshCompilerException : public std::exception {
    public:
        meshCompilerException(const std::string& message = "");
        virtual const char* what() throw();
    protected:
        std::string msg = "";
    };

// ========== COMPILE CONFIGURATION ==========

    class compileField {
    public:
        type stype;
        value vtype;
        std::vector<char> data;

        compileField(const type& s, const value& v, const void* data_source);
        compileField(const type& s, const value& v, const void* data_source, const size_t& data_amount);

        void setAsConst(const type& t, const void* data_source);
        void setData(const void* data_source, const size_t& data_amount);

        size_t get_size() const;
        void print(const int& indent = 0) const;
    };

    class compileBuffer {
    public:
        std::vector<compileField> preamble;
        size_t count = 0;
        std::vector<compileField> fields;

        size_t get_entry_size() const;
        size_t get_size() const;
        void print(const int& indent = 0) const;
        void clear();
    };

    class compileUnit {
    public:
        std::vector<compileField> preamble;
        std::vector<compileBuffer> buffers;

        compileUnit(const std::string& filename);

        size_t get_size();
        size_t get_entries_count();
        size_t get_fields_count();
        void print(const int& indent = 0) const;
        void clear();

    private:
        static type extractType(std::string& word);
        static value extractPreambleValue(std::string& word);
        static value extractFieldValue(std::string& word);
        static bool isPreambleValue(type t, std::string& arg, std::vector<compileField>& fields);
        static bool isFieldValue(type t, std::string& arg, std::vector<compileField>& fields, counting_type& field_count);
        static bool isConstValue(const type& t, std::string& arg, std::vector<compileField>& fields);
    };

    class compilationInfo {
    public:
        bool debug_messages;
        std::string output_file;
        compileUnit config;

        compilationInfo(const std::string& format_file, const std::string& output_file = "{file}_{mesh}.mesh", const bool& debug_messages = false);
    };

// ========== RUNNING METHODS ==========

public:
    static void run(int argc, char** argv);
    static void runOnce(const std::vector<std::string>& args);

private:
    static void compile(const std::vector<std::string>& args);
    static void compileFile(const std::string& filename, compilationInfo ci);
    static void compileScene(const aiScene* scene, compilationInfo ci);
    static void compileMesh(const aiMesh* m, compilationInfo ci);


    template <typename T>
    static void writeConst(std::ofstream& file, const T& value);

    template <typename T>
    static void writeConst(std::ofstream& file, const T& value, const mesh_compiler::type& type);
};

template<typename T>
inline void mesh_compiler::writeConst(std::ofstream& file, const T& value)
{
    T x = value;
    file.write((char*)&x, sizeof(T));
}

template<typename T>
inline void mesh_compiler::writeConst(std::ofstream& file, const T& value, const mesh_compiler::type& type)
{
    switch (type)
    {
    case mc_char:
        writeConst<char>(file, value);
        break;
    case mc_short:
        writeConst<short>(file, value);
        break;
    case mc_unsigned_short:
        writeConst<unsigned short>(file, value);
        break;
    case mc_int:
        writeConst<int>(file, value);
        break;
    case mc_unsigned_int:
        writeConst<unsigned int>(file, value);
        break;
    case mc_long:
        writeConst<long>(file, value);
        break;
    case mc_unsigned_long:
        writeConst<unsigned long>(file, value);
        break;
    case mc_long_long:
        writeConst<long long>(file, value);
        break;
    case mc_unsigned_long_long:
        writeConst<unsigned long long>(file, value);
        break;
    case mc_float:
        writeConst<float>(file, value);
        break;
    case mc_double:
        writeConst<double>(file, value);
        break;
    case mc_long_double:
        writeConst<long double>(file, value);
        break;
    default:
        throw std::logic_error("unknown type");
        break;
    }
}