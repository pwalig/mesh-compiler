#include "unit_testing.h"
#include "meshCompiler.h"
#include <iostream>

unit_testing::failedTestException::failedTestException(
	const std::string& test_name, const std::string& fail_reason) :
	std::logic_error("test: " + test_name + " failed: " + fail_reason) {}

unit_testing::test::test(const std::string& name) : name(name) {}

unit_testing::formatInterpreterFailTest::formatInterpreterFailTest(
	const std::string& name, const std::string& format_file, const std::string& expected_message) :
	test(name), format_file(format_file), expected_message(expected_message) {}

void unit_testing::formatInterpreterFailTest::run(const bool& for_debug)
{
	if (for_debug) mesh_compiler::compilationInfo ci(format_file);
	else {
		try {
			mesh_compiler::compilationInfo ci(format_file);
		}
		catch (mesh_compiler::formatInterpreterException& e) {
			std::string got(e.what());
			if (got != expected_message) throw failedTestException(name, "wrong format compilation error message");
			std::cout << name << " passed\n";
			return;
		}
		throw failedTestException(name, "compilation succeded, but was not supposed to");
	}
}

unit_testing::formatInterpreterSuccessTest::formatInterpreterSuccessTest(
	const std::string& name, const std::string& format_file, const info& expected) :
	test(name), format_file(format_file), expected(expected) {}

void unit_testing::formatInterpreterSuccessTest::run(const bool& for_debug)
{
	if (for_debug) mesh_compiler::compilationInfo ci(format_file);
	else {
		try {
			mesh_compiler::compilationInfo ci(format_file); 
			if (expected.file_units.size() != ci.file_units.size())
				throw failedTestException(name, "wrong amount of file units");
			if (expected.helper_units.size() != ci.units.size())
				throw failedTestException(name, "wrong amount of helper units");
			for (int i = 0; i < ci.file_units.size(); ++i) {
				if(expected.file_units[i].name != ci.file_units[i].output_file) throw failedTestException(name, "wrong file name in file unit");
			}
			for (const auto& uni: ci.units) {
				std::vector<bool> found(expected.helper_units.size(), false);
				for (int i = 0; i < expected.helper_units.size(); ++i) {
					if (expected.helper_units[i].name == uni.first) {
						if (found[i]) throw failedTestException(name, "helper unit appears more than once");
						found[i] = true;
					}
				}
				for (const bool& f : found) {
					if (!f) throw failedTestException(name, "info does not contain required helper unit");
				}

			}
		}
		catch (mesh_compiler::formatInterpreterException& e) {
			throw failedTestException(name, "format file compilation failed, but it was expected to succeed");
		}
	}
	std::cout << name << " passed\n";
}

void unit_testing::run()
{
	std::cout << "RUNNING TESTS...\n";

	formatInterpreterFailTest(
		"format-interpreter-fail-test-1",
		"./unit-tests/format-interpreter-fail/1.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::unknown_statement,
			1, "fidouaof")
	).run();

	formatInterpreterFailTest(
		"format-interpreter-fail-test-2",
		"./unit-tests/format-interpreter-fail/2.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::no_unit_name,
			1, "begin")
	).run();

	formatInterpreterFailTest(
		"format-interpreter-fail-test-3",
		"./unit-tests/format-interpreter-fail/3.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::no_file_name,
			1, "begin file")
	).run();

	formatInterpreterFailTest(
		"format-interpreter-fail-test-4",
		"./unit-tests/format-interpreter-fail/4.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::unit_redefinition,
			6, "begin a")
	).run();

	formatInterpreterFailTest(
		"format-interpreter-fail-test-5",
		"./unit-tests/format-interpreter-fail/5.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::unknown_statement,
			2, "feojpa")
	).run();

	formatInterpreterSuccessTest::info inf;
	formatInterpreterSuccessTest::unit uni = {"mesh"};
	inf.helper_units.push_back(uni);
	uni.name = "{file}_{mesh}.mesh";
	inf.file_units.push_back(uni);
	formatInterpreterSuccessTest(
		"format-interpreter-success-test-1",
		"./unit-tests/format-interpreter-success/1.format",
		inf
	).run();

	std::cout << "ALL TESTS PASSED\n";
}
