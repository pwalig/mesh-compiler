#ifdef _DEBUG
#include "unit_testing.h"
#include <sstream>

unit_testing::failedTestException::failedTestException(
	const std::string& test_name, const std::string& fail_reason) :
	std::logic_error("test: " + test_name + " failed: " + fail_reason) {}

unit_testing::test::test(const std::string& name) : name(name) {}

unit_testing::formatInterpreterFailTest::formatInterpreterFailTest(
	const std::string& name, const std::string& format_file, const std::string& expected_message) :
	test(name), format_file(format_file), expected_message(expected_message) {}

void unit_testing::formatInterpreterFailTest::run(const run_mode& mode)
{
	if (mode == run_mode::skip) std::cout << name << " skipped\n";
	else if (mode == run_mode::debug) mesh_compiler::compilationInfo ci(format_file, true);
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

unit_testing::programRunTest::programRunTest(
	const std::string& name, const std::vector<std::string>& call_arguments, const std::string& expected_response) :
	test(name), call_arguments(call_arguments), expected(expected_response) {}

void unit_testing::programRunTest::run(const run_mode& mode)
{
	if (mode == run_mode::skip) std::cout << name << " skipped\n";
	else if (mode == run_mode::debug) mesh_compiler::runOnceDebug(call_arguments);
	else {
		std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
		std::stringstream strCout;
		std::cout.rdbuf(strCout.rdbuf());

		mesh_compiler::runOnce(call_arguments);
		if (strCout.str() != expected) throw failedTestException(name, "program output was different than expected");

		std::cout.rdbuf(oldCoutStreamBuf);
		std::cout << name << " passed\n";
	}
}

void unit_testing::run()
{
	std::cout << "RUNNING TESTS...\n";

	test::run_mode mode = test::run_mode::run;

	// ========== INTERPRETER FAIL TESTS ==========

	formatInterpreterFailTest(
		"format-interpreter-fail-test-1",
		"./unit-tests/format-interpreter-fail/1.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::unknown_statement,
			1, "fidouaof")
	).run(mode);

	formatInterpreterFailTest(
		"format-interpreter-fail-test-2",
		"./unit-tests/format-interpreter-fail/2.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::no_unit_name,
			1, "begin")
	).run(mode);

	formatInterpreterFailTest(
		"format-interpreter-fail-test-3",
		"./unit-tests/format-interpreter-fail/3.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::no_file_name,
			1, "begin file")
	).run(mode);

	formatInterpreterFailTest(
		"format-interpreter-fail-test-4",
		"./unit-tests/format-interpreter-fail/4.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::unit_redefinition,
			6, "begin a")
	).run(mode);

	formatInterpreterFailTest(
		"format-interpreter-fail-test-5",
		"./unit-tests/format-interpreter-fail/5.format",
		mesh_compiler::formatInterpreterException::make_message(
			mesh_compiler::formatInterpreterException::error_code::unknown_statement,
			2, "feojpa")
	).run(mode);

	// ========== INTERPRETER SUCCESS DEEP TESTS ==========

	formatInterpreterSuccessTest<deepUnit>::info dinf;
	
	deepUnit duni1; // mesh unit
	duni1.name = "mesh";
	
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
	duni1.buffers.push_back(buff1);
	
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
	duni1.buffers.push_back(buff2);

	dinf.helper_units.push_back(duni1);

	deepUnit duni2; // file unit
	duni2.name = "{file}_{mesh}.mesh";

	char dat[4] = { 'm', 'e', 's', 'h' };
	duni2.preamble.push_back(mesh_compiler::compileField(
		mesh_compiler::type::mc_unit, mesh_compiler::value::other_unit, dat, 4) // mesh unit
	);

	dinf.file_units.push_back(duni2);

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-1",
		"./unit-tests/format-interpreter-success/deep/1-1.format",
		dinf
	).run(mode);

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-2",
		"./unit-tests/format-interpreter-success/deep/1-2.format",
		dinf
	).run(mode);

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-3",
		"./unit-tests/format-interpreter-success/deep/1-3.format",
		dinf
	).run(mode);

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-4",
		"./unit-tests/format-interpreter-success/deep/1-4.format",
		dinf
	).run(mode);

	formatInterpreterSuccessTest<deepUnit>(
		"format-interpreter-success-deep-test-1-5",
		"./unit-tests/format-interpreter-success/deep/1-5.format",
		dinf
	).run(mode);

	// ========== INTERPRETER SUCCESS TESTS ==========

	formatInterpreterSuccessTest<unit>::info inf;
	unit uni1;
	uni1.name = "mesh";
	uni1.preamble = 0;
	uni1.buffers.push_back(4); // fieldb indice
	uni1.buffers.push_back(5); // fieldb vertex float
	uni1.buffers.push_back(3); // fieldb uv.0.x uv.0.y
	uni1.buffers.push_back(4); // fieldb normal
	inf.helper_units.push_back(uni1);
	unit uni2;
	uni2.name = "abc";
	uni2.preamble = 1; // fields
	uni2.buffers.push_back(4); // entryb indice
	uni2.buffers.push_back(4); // entryb vertex
	uni2.buffers.push_back(4); // entryb normal
	inf.helper_units.push_back(uni2);
	unit uni3;
	uni3.name = "{file}_{mesh}.mesh";
	uni3.preamble = 1; // mesh
	uni3.buffers.push_back(4); // fieldb tangent
	uni3.buffers.push_back(4); // fieldb bitangent
	inf.file_units.push_back(uni3);
	unit uni4;
	uni4.name = "{scene}-{mesh}.mesh";
	uni4.preamble = 0;
	uni4.buffers.push_back(2); // entryb abc
	inf.file_units.push_back(uni4);

	formatInterpreterSuccessTest<unit>(
		"format-interpreter-success-test-1-1",
		"./unit-tests/format-interpreter-success/unit/1-1.format",
		inf
	).run(mode);

	formatInterpreterSuccessTest<unit>(
		"format-interpreter-success-test-1-2",
		"./unit-tests/format-interpreter-success/unit/1-2.format",
		inf
	).run(mode);

	formatInterpreterSuccessTest<unit>(
		"format-interpreter-success-test-1-3",
		"./unit-tests/format-interpreter-success/unit/1-3.format",
		inf
	).run(mode);

	formatInterpreterSuccessTest<unit>(
		"format-interpreter-success-test-1-4",
		"./unit-tests/format-interpreter-success/unit/1-4.format",
		inf
	).run(mode);

	formatInterpreterSuccessTest<unit>(
		"format-interpreter-success-test-1-5",
		"./unit-tests/format-interpreter-success/unit/1-5.format",
		inf
	).run(mode);

	// ========== INTERPRETER SUCCESS SHALLOW TESTS ==========

	formatInterpreterSuccessTest<shallowUnit>::info sinf;
	sinf.helper_units.push_back({"animation_channel"});
	sinf.helper_units.push_back({"animation"});
	sinf.helper_units.push_back({"skeleton"});
	sinf.helper_units.push_back({"mesh"});
	sinf.file_units.push_back({"{file}{mesh}.mesh"});
	sinf.file_units.push_back({"{file}{skeleton}.skel"});
	sinf.file_units.push_back({"{file}{animation}.anim"});

	formatInterpreterSuccessTest<shallowUnit>(
		"format-interpreter-success-shallow-test-1-1",
		"./unit-tests/format-interpreter-success/shallow/1-1.format",
		sinf
	).run(mode);

	formatInterpreterSuccessTest<shallowUnit>(
		"format-interpreter-success-shallow-test-1-2",
		"./unit-tests/format-interpreter-success/shallow/1-2.format",
		sinf
	).run(mode);

	// ========== EXPORTED FILE TESTS ==========

	bufferedObject<float, unsigned int> obj;
	obj.preamble.push_back(2);
	preambledBuffer<float, unsigned int> buff;
	buff.preamble.push_back(15);
	buff.fields = { 1, 0, 1, 1, 0, -1, -1, 0, -1, -1, 0, -2, -1, 0, 1 };
	obj.buffers.push_back(buff);
	buff.fields = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	obj.buffers.push_back(buff);
	meshCompilerTest<float, unsigned int>(
		"mesh-compiler-test-1",
		"./unit-tests/mesh-compiler/1/test.obj",
		"./unit-tests/mesh-compiler/1/.format",
		obj
	).run(mode);
	

	// ========== PROGRAM RUN TESTS ==========

	programRunTest(
		"program-run-test-1",
		{"-v"},
		mesh_compiler::version + "\n"
	).run(mode);

	programRunTest(
		"program-run-test-2",
		{"--version"},
		mesh_compiler::version + "\n"
	).run(mode);

	programRunTest(
		"program-run-test-3",
		{},
		"source file not specified\n"
	).run(mode);

	programRunTest(
		"program-run-test-4",
		{ "yeet.fbx", "-f" },
		"unspecified format file: -f <format file path>\n"
	).run(mode);

	programRunTest(
		"program-run-test-5",
		{ "cube.obj", "-o" },
		"unspecified output file: -o <output file path>\n"
	).run(mode);

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

#endif _DEBUG