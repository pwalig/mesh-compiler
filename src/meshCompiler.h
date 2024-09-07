#pragma once
#include <string>
#include <vector>
#include <map>
#include <assimp/scene.h>

namespace mesh_compiler
{
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

    class compileField {
    public:
        char type;
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

	void run(int argc, char** argv);
	void runOnce(const std::vector<std::string>& args);
	void compile(const std::vector<std::string>& args);
    void compileFile(const std::string& filename, compilationInfo ci);
    void compileScene(const aiScene* scene, compilationInfo ci);
    void compileMesh(const aiMesh* m, compilationInfo ci);
}