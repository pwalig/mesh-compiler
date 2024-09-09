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

        mc_unit,
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
    
    enum class value {
        null,

        constant,
        other_unit,
        indice,
        vertex,
        normal,
        tangent,
        bitangent,
        uv,
        vertex_color,
        position_key,
        rotation_key,
        scale_key,
        timestamp,
        duration,
        ticks_per_second,

        unit_size,
        buffer_size,
        buffers_per_unit,
        entry_size,
        entries_per_unit,
        entries_per_buffer,
        field_size,
        fields_per_unit,
        fields_per_buffer,
        fields_per_entry
    };
    
    enum class counting_type {
        null,

        per_indice,
        per_vertex,
        per_mesh_bone,
        per_mesh,

        per_bone,
        per_skeleton,

        per_keyframe,
        per_animation_channel,
        per_animation,

        per_scene
    };

    static type getDefaultValueType(const value& v);
    static counting_type getFieldCount(const value& t);
    static counting_type getParentCountingType(const counting_type& ct);
    static std::vector<unsigned short> getMaxSuffixes(const value& t);
    static void copyConstantToMemory(void* dst, const type& type, const std::string& val);

    static std::map<std::string, value> preambleMap;
    static std::map<std::string, value> fieldsMap;
    static std::map<std::string, type> typesMap;
    static std::map<type, unsigned short> typeSizesMap;

    static std::map<type, std::string> typeNamesMap;
    static std::map<value, std::string> valueNamesMap;
    static std::map<counting_type, std::string> countingTypeNamesMap;

// ========== EXCEPTIONS ==========

    class formatInterpreterException : public std::exception {
    public:
        enum class error_code {
            cannot_open_file,
            unknown_statement,
            no_suffix,
            invalid_suffix,
            wrong_suffixes_amount,
            no_const_value,
            invalid_const_value,
            invalid_type_specifier,
            byte_base_in_count_type,
            field_spec_in_preamble,
            unsupported_type,
            conflicting_buffer_fields,
            conflicting_unit_fields,
            constants_only,
            no_unit_name,
            no_file_name,
            unit_redefinition,
            no_end,
            unknown
        };
        formatInterpreterException(const error_code& error_code, const std::string& message = "");
        formatInterpreterException(const error_code& error_code, const unsigned int& line_number, const std::string& processed_word, const std::string& message = "");
        void fillInfo(const unsigned int& line_number, const std::string& processed_word);
        virtual const char* what() throw();
    protected:
        error_code type;
        bool filled;
        std::string msg = "";
    private:
        static std::map<error_code, std::string> errorMessagesMap;
    };

    class meshCompilerException : public std::exception {
    public:
        meshCompilerException(const std::string& message = "");
        virtual const char* what() throw();
    protected:
        std::string msg = "";
    };

// ========== COMPILE CONFIGURATION ==========

    class compileBuffer;
    class compileUnit;

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
        std::string get_otherUnitName() const;
        void print(const int& indent = 0) const;

        void put(std::ofstream& file, const compileBuffer& buffer) const;
        void put(std::ofstream& file, const std::vector<compileBuffer>& buffers) const;
        void put(std::ofstream& file, const compileUnit& unit) const;
    };

    class compileBuffer {
    public:
        std::vector<compileField> preamble;
        std::vector<compileField> fields;
        size_t count = 0;
        counting_type count_type = counting_type::null;

        size_t get_entry_size() const;
        size_t get_size() const;
        void print(const int& indent = 0) const;
        void clear();
    };

    class compileUnit {
    public:
        std::vector<compileField> preamble;
        std::vector<compileBuffer> buffers;
        counting_type count_type = counting_type::null;
        std::map<std::string, compileUnit>* unitsMap = nullptr;

        compileUnit() = default;
        compileUnit(std::ifstream& file, size_t& line_num, /*const*/ std::map<std::string, compileUnit>* unitsMap);

        size_t get_size() const;
        size_t get_entries_count() const;
        size_t get_fields_count() const;
        void print(const int& indent = 0) const;
        void clear();

        void put(std::ofstream& file, const aiNodeAnim* animation_channel);
        void put(std::ofstream& file, const aiSkeleton* skeleton);
        void put(std::ofstream& file, const aiAnimation* animation);
        void put(std::ofstream& file, const aiMesh* mesh);
        void put(std::ofstream& file, const aiScene* scene);

    private:
        static type extractType(std::string& word);
        static value extractPreambleValue(std::string& word);
        static value extractFieldValue(std::string& word);
        static bool isPreambleValue(type t, std::string& arg, std::vector<compileField>& fields);
        static bool isFieldValue(type t, std::string& arg, std::vector<compileField>& fields, counting_type& field_count, counting_type& unit_count);
        static bool isConstValue(const type& t, std::string& arg, std::vector<compileField>& fields);
        static bool isOtherUnitValue(const type& t, std::string& arg, std::vector<compileField>& fields, counting_type& count_type, /*const*/ std::map<std::string, compileUnit>& unitsMap);
    };

    class fileUnit : public compileUnit {
    public:
        std::string output_file;

        fileUnit(std::ifstream& file, const std::string& output_file_, size_t& line_num, /*const*/ std::map<std::string, compileUnit>* unitsMap);
    };

    class compilationInfo {
    public:
        bool debug_messages;
        std::map<std::string, compileUnit> units;
        std::vector<fileUnit> file_units;

        compilationInfo(const std::string& format_file, const bool& debug_messages = false);
    };

// ========== RUNNING METHODS ==========

public:
    static void run(int argc, char** argv);
    static void runOnce(const std::vector<std::string>& args);

private:
    static void compile(const std::vector<std::string>& args);
    static void compileFile(const std::string& filename, compilationInfo ci);
    static void compileScene(const aiScene* scene, fileUnit ci);


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