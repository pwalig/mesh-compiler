#pragma once
#include <string>
#include <vector>
#include <map>
#include <assimp/scene.h>

class mesh_compiler {
public:
    mesh_compiler() = delete;
    mesh_compiler(const mesh_compiler& other) = delete;
    mesh_compiler(mesh_compiler&& other) = delete;

private:
    class formatInterpreterException : public std::exception {
    public:
        enum error_code {
            mc_err_cannot_open_file,
            mc_err_unknown_statement,
            mc_err_no_suffix,
            mc_err_invalid_suffix,
            mc_err_no_const_value,
            mc_err_invalid_const_value,
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

    enum type {
        mc_char,
        mc_short, mc_unsigned_short,
        mc_int, mc_unsigned_int,
        mc_long, mc_unsigned_long,
        mc_long_long, mc_unsigned_long_long,
        mc_float, mc_double, mc_long_double,
        mc_indice,
        mc_vertex,
        mc_normal,
        mc_tangent,
        mc_bitangent,
        mc_uv,
        mc_vertex_color,
        mc_file_size,
        mc_buffer_size,
        mc_buffers_per_unit,
        mc_entry_size,
        mc_entries_per_unit,
        mc_entries_per_buffer,
        mc_field_size,
        mc_fields_per_unit,
        mc_fields_per_entry,
        mc_fields_per_buffer,
    };
    static char getFieldCount(const type& t);
    static void copyConstantToMemory(void* dst, const type& type, const std::string& val);

    static std::map<std::string, type> preambleMap;
    static std::map<std::string, type> fieldsMap;
    static std::map<std::string, type> constsMap;
    static std::map<type, unsigned short> typeSizesMap;

    class compileField {
    public:
        type type;
        std::vector<char> data;

        unsigned int get_size(unsigned short byte_base = 1) const;
        void print(const int& indent = 0) const;
    };

    class compilePreamble {
    public:
        std::vector<char> info_format;
        std::vector<char> data;

        void print(const int& indent = 0) const;
        void clear();
    };

    class compileBuffer {
    public:
        compilePreamble preamble;
        size_t count = 0;
        std::vector<compileField> fields;

        unsigned int get_entry_size(unsigned short byte_base = 1) const;
        unsigned int get_size(unsigned short byte_base = 1) const;
        void print(const int& indent = 0) const;
        void clear();
    };

    class compileConfig {
    public:
        compilePreamble preamble;
        std::vector<compileBuffer> buffers;

        compileConfig(const std::string& filename);

        unsigned int get_size(unsigned short byte_base = 1);
        unsigned int get_entries_count();
        unsigned int get_fields_count();
        void print(const int& indent = 0) const;
        void clear();

    private:
        bool isArgument(const std::string& arg, compilePreamble& preamble);
        bool isField(const std::string& arg, std::vector<compileField>& fields, char& field_count);
        bool isType(const std::string& arg, compilePreamble& preamble);
        bool isType(const std::string& arg, std::vector<compileField>& fields);
    };

    class compilationInfo {
    public:
        bool debug_messages;
        std::string output_file;
        compileConfig config;

        compilationInfo(const std::string& format_file, const std::string& output_file = "{file}_{mesh}.mesh", const bool& debug_messages = false);
    };

public:
    static void run(int argc, char** argv);
    static void runOnce(const std::vector<std::string>& args);

private:
    static void compile(const std::vector<std::string>& args);
    static void compileFile(const std::string& filename, compilationInfo ci);
    static void compileScene(const aiScene* scene, compilationInfo ci);
    static void compileMesh(const aiMesh* m, compilationInfo ci);
};