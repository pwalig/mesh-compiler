#include "unit_testing.h"
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
	if (for_debug) mesh_compiler::compilationInfo ci(format_file, true);
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
	if (for_debug) mesh_compiler::compilationInfo ci(format_file, true);
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
			for (const auto& uni : ci.units) {
				std::vector<bool> found(expected.helper_units.size(), false);
				for (int i = 0; i < expected.helper_units.size(); ++i) {
					if (expected.helper_units[i].name == uni.first) {
						if (found[i]) throw failedTestException(name, "helper unit found twice");
						found[i] = true;
						if (expected.helper_units[i] != uni.second) throw failedTestException(name, "helper unit differs in structure");
					}
				}
				for (const bool& f : found) {
					if (!f) throw failedTestException(name, "expected helper unit not found");
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
	
	formatInterpreterSuccessTest::unit uni1; // mesh unit
	uni1.name = "mesh";
	
	mesh_compiler::compileBuffer buff1; // index buffer
	buff1.count_type = mesh_compiler::counting_type::per_indice;
	buff1.preamble.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unsigned_int, mesh_compiler::value::fields_per_buffer, nullptr)
	);
	char val = 0;
	buff1.fields.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unsigned_int, mesh_compiler::value::indice, &val, 1)
	);
	val = 1;
	buff1.fields.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unsigned_int, mesh_compiler::value::indice, &val, 1)
	);
	val = 2;
	buff1.fields.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unsigned_int, mesh_compiler::value::indice, &val, 1)
	);
	uni1.buffers.push_back(buff1);
	
	mesh_compiler::compileBuffer buff2; // vertex buffer
	buff2.count_type = mesh_compiler::counting_type::per_vertex;
	buff2.preamble.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unsigned_int, mesh_compiler::value::fields_per_buffer, nullptr)
	);
	val = 0;
	buff2.fields.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_float, mesh_compiler::value::vertex, &val, 1)
	);
	val = 1;
	buff2.fields.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_float, mesh_compiler::value::vertex, &val, 1)
	);
	val = 2;
	buff2.fields.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_float, mesh_compiler::value::vertex, &val, 1)
	);
	uni1.buffers.push_back(buff2);

	inf.helper_units.push_back(uni1);

	formatInterpreterSuccessTest::unit uni2; // file unit
	uni2.name = "{file}_{mesh}.mesh";

	char dat[4] = { 'm', 'e', 's', 'h' };
	uni2.preamble.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unit, mesh_compiler::value::other_unit, dat, 4) // mesh unit
	);

	inf.file_units.push_back(uni2);

	formatInterpreterSuccessTest(
		"format-interpreter-success-test-1",
		"./unit-tests/format-interpreter-success/1.format",
		inf
	).run();

	std::cout << "ALL TESTS PASSED\n";
}

bool unit_testing::formatInterpreterSuccessTest::info::operator==(const mesh_compiler::compilationInfo& other)
{
	// file units
	if (this->file_units.size() != other.file_units.size()) return false;
	for (int i = 0; i < other.file_units.size(); ++i) {
		if (this->file_units[i].name != other.file_units[i].output_file) return false;
		if (this->file_units[i] != other.file_units[i]) return false;
	}

	// helper units
	if (this->helper_units.size() != other.units.size()) return false;
	for (const auto& uni : other.units) {
		std::vector<bool> found(this->helper_units.size(), false);
		for (int i = 0; i < this->helper_units.size(); ++i) {
			if (this->helper_units[i].name == uni.first) {
				if (found[i]) return false;
				found[i] = true;
				if (this->helper_units[i] != uni.second) return false;
			}
		}
		for (const bool& f : found) {
			if (!f) return false;
		}

	}
	return true;
}

bool unit_testing::formatInterpreterSuccessTest::info::operator!=(const mesh_compiler::compilationInfo& other)
{
	return !(*this == other);
}

bool unit_testing::formatInterpreterSuccessTest::unit::operator==(const mesh_compiler::compileUnit& other)
{
	return (this->preamble == other.preamble && this->buffers == other.buffers);
}

bool unit_testing::formatInterpreterSuccessTest::unit::operator!=(const mesh_compiler::compileUnit& other)
{
	return !(*this == other);
}
