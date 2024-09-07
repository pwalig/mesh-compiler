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
        formatInterpreterException(const int& error_code, const std::string& message = "");
        void fillInfo(const unsigned int& line_number, const std::string& processed_word);
        virtual const char* what() throw();
    protected:
        int type;
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
        mc_vertex_color
    };
    static char getFieldCount(const type& t);
    static void copyConstantToMemory(void* dst, const type& type, const std::string& val);

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