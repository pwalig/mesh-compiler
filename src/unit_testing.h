#pragma once
#ifdef _DEBUG
#include <vector>
#include <fstream>
#include <iostream>
#include "meshReader.h"
#include "meshCompiler.h"

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

        preambledBuffer() = default;
        preambledBuffer(std::ifstream& file);
        bool operator==(const preambledBuffer<T, U>& other);
        bool operator!=(const preambledBuffer<T, U>& other);
    };
    
    template <typename T, typename U>
    class bufferedObject {
    public:
        std::vector<U> preamble;
        std::vector<preambledBuffer<T, U>> buffers;


        bufferedObject() = default;
        bufferedObject(std::ifstream& file);
        bool operator==(const bufferedObject<T, U>& other);
        bool operator!=(const bufferedObject<T, U>& other);
    };

    class test {
    public:
        enum class run_mode {
            run,
            debug,
            skip
        };
        std::string name;
        test(const std::string& name);
        virtual void run(const run_mode& mode = run_mode::run) = 0;
    };

    class formatInterpreterFailTest : public test {
    public:
        std::string format_file;
        std::string expected_message;
        formatInterpreterFailTest(const std::string& name, const std::string& format_file, const std::string& expected_message);
        void run(const run_mode& mode = run_mode::run) override;
    };

    class deepUnit {
    public:
        std::string name;
        std::vector<mesh_compiler::compileField> preamble;
        std::vector<mesh_compiler::compileBuffer> buffers;

        bool operator==(const mesh_compiler::compileUnit& other);
        bool operator!=(const mesh_compiler::compileUnit& other);
    };

    class unit {
    public:
        std::string name;
        size_t preamble;
        std::vector<size_t> buffers;

        bool operator==(const mesh_compiler::compileUnit& other);
        bool operator!=(const mesh_compiler::compileUnit& other);
    };

    class shallowUnit {
    public:
        std::string name;

        bool operator==(const mesh_compiler::compileUnit& other);
        bool operator!=(const mesh_compiler::compileUnit& other);
    };

    template <typename T>
    class formatInterpreterSuccessTest : public test {
    public:
        class info {
        public:
            std::vector<T> file_units;
            std::vector<T> helper_units;
        };
        std::string format_file;
        info expected;
        formatInterpreterSuccessTest(const std::string& name, const std::string& format_file, const info& expected);
        void run(const run_mode& mode = run_mode::run) override;
    };

    template <typename T, typename U>
    class meshCompilerTest : public test {
    public:
        bufferedObject<T, U> expected;
        std::vector<std::string> call_arguments;
        meshCompilerTest(const std::string& name, const std::string& input_file, const std::string& format_file, const bufferedObject<T, U>& expected_output);
        void run(const run_mode& mode = run_mode::run) override;
    };

    class programRunTest : public test {
    public:
        std::vector<std::string> call_arguments;
        std::string expected;
        programRunTest(const std::string& name, const std::vector<std::string>& call_arguments, const std::string& expected_response);
        void run(const run_mode& mode = run_mode::run);
    };

    static void run();
};

template<typename T, typename U>
inline unit_testing::preambledBuffer<T, U>::preambledBuffer(std::ifstream& file)
{
    preamble.resize(1);
    file.read((char*)preamble.data(), sizeof(U) * preamble.size());

    mesh_reader::readBuffer<T, U>(file, fields, preamble[0]);
}

template<typename T, typename U>
inline bool unit_testing::preambledBuffer<T, U>::operator==(const preambledBuffer<T, U>& other)
{
    return (this->preamble == other.preamble && this->fields == other.fields);
}

template<typename T, typename U>
inline bool unit_testing::preambledBuffer<T, U>::operator!=(const preambledBuffer<T, U>& other)
{
    return !(*this == other);
}

template<typename T, typename U>
inline unit_testing::bufferedObject<T, U>::bufferedObject(std::ifstream& file)
{
    preamble.resize(1);
    file.read((char*)preamble.data(), sizeof(U) * preamble.size());

    for (int i = 0; i < preamble[0]; ++i) {
        buffers.push_back(preambledBuffer<T, U>(file));
    }
}

template<typename T, typename U>
inline bool unit_testing::bufferedObject<T, U>::operator==(const bufferedObject<T, U>& other)
{
    if (this->buffers.size() != other.buffers.size())
    for (int i = 0; i < this->buffers.size(); ++i) {
        if (this->buffers[i] != other.buffers[i]) return false;
    }
    return (this->preamble == other.preamble);
}

template<typename T, typename U>
inline bool unit_testing::bufferedObject<T, U>::operator!=(const bufferedObject<T, U>& other)
{
    return !(*this == other);
}

template <typename T>
unit_testing::formatInterpreterSuccessTest<T>::formatInterpreterSuccessTest(
    const std::string& name, const std::string& format_file, const info& expected) :
    test(name), format_file(format_file), expected(expected) {}

template <typename T>
void unit_testing::formatInterpreterSuccessTest<T>::run(const run_mode& mode)
{
    if (mode == run_mode::skip) std::cout << name << " skipped\n";
    else if (mode == run_mode::debug) mesh_compiler::compilationInfo ci(format_file, true);
    else {
        try {
            mesh_compiler::compilationInfo ci(format_file);
            //if (expected != ci) throw failedTestException(name, "resulting compilation info differs from expected");

            // file units
            if (expected.file_units.size() != ci.file_units.size()) throw failedTestException(name, "different amounts of file units");
            for (int i = 0; i < ci.file_units.size(); ++i) {
                if (expected.file_units[i].name != ci.file_units[i].output_file) throw failedTestException(name, "different file unit file name");
                if (expected.file_units[i] != ci.file_units[i]) throw failedTestException(name, "file unit differs in structure");
            }

            // helper units
            if (expected.helper_units.size() != ci.units.size()) throw failedTestException(name, "different amount of helper units");
            std::vector<bool> found(expected.helper_units.size(), false);
            for (const auto& uni : ci.units) {
                for (int i = 0; i < expected.helper_units.size(); ++i) {
                    if (expected.helper_units[i].name == uni.first) {
                        if (found[i]) throw failedTestException(name, "helper unit found twice");
                        found[i] = true;
                        if (expected.helper_units[i] != uni.second) throw failedTestException(name, "helper unit differs in structure");
                    }
                }
            }
            for (const bool& f : found) {
                if (!f) throw failedTestException(name, "expected helper unit not found");
            }

        }
        catch (mesh_compiler::formatInterpreterException& e) {
            throw failedTestException(name, "format file compilation failed, but it was expected to succeed");
        }
        std::cout << name << " passed\n";
    }
}

template<typename T, typename U>
inline unit_testing::meshCompilerTest<T, U>::meshCompilerTest(
    const std::string& name, const std::string& input_file, const std::string& format_file, const bufferedObject<T, U>& expected_output) :
    test(name), call_arguments({input_file, format_file}), expected(expected_output) {}

template<typename T, typename U>
inline void unit_testing::meshCompilerTest<T, U>::run(const run_mode& mode)
{
    if (mode == run_mode::skip) std::cout << name << " skipped\n";
    else if (mode == run_mode::debug) mesh_compiler::runOnceDebug(call_arguments);
    else {
        mesh_compiler::runOnce(call_arguments);

        std::ifstream file("./unit-tests/mesh-compiler/out.mesh", std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error("could not open file: ./unit-tests/mesh-compiler/out.mesh");
        }
        bufferedObject<T, U> output(file);
        file.close();
        std::remove("./unit-tests/mesh-compiler/out.mesh");
        if (output != expected) throw failedTestException(name, "read object differs from expected");

        std::cout << name << " passed\n";
    }
}

#endif // _DEBUG