#include "unit_testing.h"

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

void unit_testing::run()
{
	std::cout << "RUNNING TESTS...\n";

	// ========== INTERPRETER FAIL TESTS ==========

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

	// ========== INTERPRETER SUCCESS DEEP TESTS ==========

	formatInterpreterSuccessTest<deepUnit>::info inf;
	
	deepUnit uni1; // mesh unit
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

	deepUnit uni2; // file unit
	uni2.name = "{file}_{mesh}.mesh";

	char dat[4] = { 'm', 'e', 's', 'h' };
	uni2.preamble.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unit, mesh_compiler::value::other_unit, dat, 4) // mesh unit
	);

	inf.file_units.push_back(uni2);

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-1",
		"./unit-tests/format-interpreter-success/deep/1.format",
		inf
	).run();

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-2",
		"./unit-tests/format-interpreter-success/deep/2.format",
		inf
	).run();

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-3",
		"./unit-tests/format-interpreter-success/deep/3.format",
		inf
	).run();

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-4",
		"./unit-tests/format-interpreter-success/deep/4.format",
		inf
	).run();

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-5",
		"./unit-tests/format-interpreter-success/deep/5.format",
		inf
	).run();

	// ========== INTERPRETER SUCCESS TESTS ==========


	// ========== INTERPRETER SUCCESS SHALLOW TESTS ==========


	// ========== EXPORTED FILE TESTS ==========

	std::cout << "ALL TESTS PASSED\n";
}

bool unit_testing::deepUnit::operator==(const mesh_compiler::compileUnit& other)
{
	return (this->preamble == other.preamble && this->buffers == other.buffers);
}

bool unit_testing::deepUnit::operator!=(const mesh_compiler::compileUnit& other)
{
	return !(*this == other);
}

bool unit_testing::unit::operator==(const mesh_compiler::compileUnit& other)
{
	if (this->buffers.size() != other.buffers.size()) return false;
	for (size_t i = 0; i < this->buffers.size(); ++i) {
		if (this->buffers[i] != (other.buffers[i].fields.size() + other.buffers[i].preamble.size()))
			return false;
	}
	return (this->preamble == other.preamble.size());
}

bool unit_testing::unit::operator!=(const mesh_compiler::compileUnit& other)
{
	return !(*this == other);
}

bool unit_testing::shallowUnit::operator==(const mesh_compiler::compileUnit& other)
{
	return true;
}

bool unit_testing::shallowUnit::operator!=(const mesh_compiler::compileUnit& other)
{
	return false;
}
