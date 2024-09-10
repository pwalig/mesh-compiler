#pragma once
#include <vector>
#include <fstream>
#include "meshReader.h"

class unit_testing
{
public:
    unit_testing() = delete;
    unit_testing(const unit_testing& other) = delete;
    unit_testing(unit_testing&& other) = delete;

    class failedTestException : public std::logic_error {
    public:
        failedTestException(const std::string& test_name, const std::string& fail_reason);
    };

    template <typename T, typename U>
    class preambledBuffer {
    public:
        std::vector<U> preamble;
        std::vector<T> fields;

        preambledBuffer(std::ifstream& file, const size_t& other_values_count = 0); // reads buffer size(true) or field count(false), then other values, then fields
    };
    
    template <typename T, typename U>
    class bufferedObject {
    public:
        std::vector<U> preamble;
        std::vector<preambledBuffer<T, U>> buffers;


        bufferedObject(std::ifstream& file, const size_t& other_preamble_values_count = 0);
    };

    class test {
    public:
        std::string name;
        test(const std::string& name);
        virtual void run(const bool& for_debug = false) = 0;
    };

    class formatInterpreterFailTest : public test {
    public:
        std::string format_file;
        std::string expected_message;
        formatInterpreterFailTest(const std::string& name, const std::string& format_file, const std::string& expected_message);
        void run(const bool& for_debug = false) override;
    };

    class formatInterpreterSuccessTest : public test {
    public:
        struct unit {
            std::string name;
            //std::vector<mesh_compiler::compileField> preamble;
            //std::vector< std::vector<mesh_compiler::compileField>> buffers;
        };
        struct info {
            std::vector<unit> file_units;
            std::vector<unit> helper_units;
        };
        std::string format_file;
        info expected;
        formatInterpreterSuccessTest(const std::string& name, const std::string& format_file, const info& expected);
        void run(const bool& for_debug = false) override;
    };

    static void run();
};

template<typename T, typename U>
inline unit_testing::preambledBuffer<T, U>::preambledBuffer(std::ifstream& file, const size_t& other_values_count)
{
    preamble.resize(other_values_count + 1);
    file.read((char*)preamble.data(), sizeof(U) * preamble.size());

    mesh_reader::readBuffer<T, U>(file, fields, preamble[0]);
}

template<typename T, typename U>
inline unit_testing::bufferedObject<T, U>::bufferedObject(std::ifstream& file, const size_t& other_preamble_values_count)
{
    preamble.resize(other_preamble_values_count + 1);
    file.read((char*)preamble.data(), sizeof(U) * preamble.size());

    for (int i = 0; i < preamble[0]; ++i) {
        buffers.push_back(preambledBuffer(file));
    }
}
