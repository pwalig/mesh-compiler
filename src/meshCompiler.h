#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>

namespace mesh_compiler
{
    class compileField {
    public:
        char type;
        char data[4];

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

        int compile(const std::string& filename);

        unsigned int get_size(unsigned short byte_base = 1);
        unsigned int get_entries_count();
        unsigned int get_fields_count();
        void print(const int& indent = 0) const;
        void clear();

    private:
        std::string error_message = "";
        void logCompilationError(const int& err_code, const std::string& arg, const int& line_num);
        int isArgument(const std::string& arg, compilePreamble& preamble);
        int isField(const std::string& arg, std::vector<compileField>& fields, char& field_count);
        int isType(const std::string& arg, compilePreamble& preamble);
        int isType(const std::string& arg, std::vector<compileField>& fields);
    };

    class compilationInfo {
    public:
        bool indices = true;
        bool debug_messages = false;
        std::string format_file = ".format";
        std::string output_file = "{file}_{mesh}.mesh";

    public:
        int updateCompileConfig();
        compileConfig* config = nullptr;

        ~compilationInfo();
    };

	void run(int argc, char** argv);
	int compile(const std::vector<std::string>& args);
	int compileFile(const std::string& filename, compilationInfo& ci);
	int compileScene(const aiScene* scene, compilationInfo& ci);
    int compileMesh(const aiMesh* m, compilationInfo& ci);
}