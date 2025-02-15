#include "testing.h"
#include <iostream>
#include "../core/exceptions/jsonException.h"
#include "../core/exceptions/compileException.h"
#include "../core/compilation-info.h"
#include "../core/fields/vfieldT.h"
#include "../core/fields/pfieldT.h"
#include "../core/fields/cfieldT.h"
#include "../core/fields/cfield.h"
#include "../core/fields/vfield.h"
#include "../core/fields/pfield.h"
#include "../core/fields/ufield.h"
#include "../core/exceptions/formatException.h"

void tests::run(
    const std::vector<Case> cases,
    const std::vector<std::string>& referenceFiles,
    const std::string& sourceFile
) {
    std::vector<std::ifstream> references;
    std::vector<std::streampos> fileSizes;
    references.reserve(referenceFiles.size());
    fileSizes.reserve(referenceFiles.size());

    for (const std::string& filename : referenceFiles) {
        references.push_back(std::ifstream(filename, std::ifstream::binary | std::ifstream::ate));
        if (references.back().fail()) {
            std::cout << "could not open reference file: " + filename + "\n";
            return;
        }
        fileSizes.push_back(references.back().tellg());
        references.back().seekg(0, std::ifstream::beg);
    }

    for (size_t i = 0; i < cases.size(); ++i) {
        std::cout << "\tcase #" << i;
        try {
            mc::compilationInfo ci_j = cases[i].getCompilationInfo();
            ci_j.compileFile(sourceFile);
            mc::compilationInfo::units.clear();
        }
        catch (mc::jsonException& je) {
            std::cout << " failed via json exception: " << je.what() << "\n";
            continue;
        }
        catch (mc::formatException& fe) {
            std::cout << " failed via format exception: in line " << fe.context.linenum << " " << fe.what() << "\n";
            continue;
        }
        catch (mc::compileException& ce) {
            std::cout << " failed via compile exception: " << ce.what() << "\n";
            continue;
        }

        if (referenceFiles.size() != cases[i].resultingFiles.size()) {
            std::cout << " failed via file comparison:\nthere was different amount of resulting files than reference files\n";
            continue;
        }
        std::cout << "\n";

        for (size_t j = 0; j < referenceFiles.size(); ++j) {
            std::ifstream result(cases[i].resultingFiles[j], std::ifstream::binary | std::ifstream::ate);
            std::cout << "\t\t" << cases[i].resultingFiles[j] << " vs " << referenceFiles[j];
            if (result.fail()) {
                std::cout << " failed: could not open\n";
                continue;
            }

            if (result.tellg() != fileSizes[j]) {
                std::cout << " failed: had different size\n";
                continue;
            }
            result.seekg(0, std::ifstream::beg);

            if (!std::equal(
                    std::istreambuf_iterator<char>(references[j].rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(result.rdbuf())
            )) {
                std::cout << " failed: had different contents\n";
                continue;
            }

            std::cout << " succeeded\n";
        }
    }
}

void tests::run()
{
    std::cout << "ACCEPTANCE TESTS\n";
    std::cout << "test #1\n";
    run({
        Case(Case::getFromFile("tests/acceptance/1format.json"), {"tests/acceptance/res/Cube_json.txt"}),

        Case([]() {
            mc::compilationInfo ci_c;
            mc::fileUnit fu;
            fu.c = mc::ctype::per_mesh;
            fu.output_file = "tests/acceptance/res/{mesh}_code_templates.txt";
            fu.mode = mc::printMode::plainText;
            fu.preamble.push_back(mc::field::ptr(new mc::cfieldT<int>(-50)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::buffers_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::buffer_size)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::entries_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::entries_per_buffer)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::entry_size)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::fields_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::fields_per_buffer)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::fields_per_entry)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::field_size)));
            mc::buffer buff;
            buff.c = mc::ctype::per_vertex;
            buff.preamble.push_back(mc::field::ptr(new mc::cfieldT<int>(-30)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::buffer_size)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::entries_per_buffer)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::entry_size)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::fields_per_buffer)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::fields_per_entry)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::field_size)));
            buff.fields.push_back(mc::field::ptr(new mc::vfieldT<float>(mc::vtype::vertex, { })));
            buff.fields.push_back(mc::field::ptr(new mc::cfieldT<int>(-20)));
            fu.buffers.push_back(buff);
            fu.buffers.push_back(buff);
            ci_c.file_units.push_back(fu);
            return ci_c;
        }, {"tests/acceptance/res/Cube_code_templates.txt"}),

        Case([]() {
            mc::compilationInfo ci_c;
            mc::fileUnit fu;
            fu.c = mc::ctype::per_mesh;
            fu.output_file = "tests/acceptance/res/{mesh}_code.txt";
            fu.mode = mc::printMode::plainText;
            fu.preamble.push_back(mc::field::ptr(new mc::cfield(mc::stype::int4, mc::anyType::getValue(mc::stype::int4, -50))));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::buffers_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::buffer_size)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::entries_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::entries_per_buffer)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::entry_size)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::fields_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::fields_per_buffer)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::fields_per_entry)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::field_size)));
            mc::buffer buff;
            buff.c = mc::ctype::per_vertex;
            buff.preamble.push_back(mc::field::ptr(new mc::cfield(mc::stype::int4, mc::anyType::getValue(mc::stype::int4, -30))));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::buffer_size)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::entries_per_buffer)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::entry_size)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::fields_per_buffer)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::fields_per_entry)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::field_size)));
            buff.fields.push_back(mc::field::ptr(new mc::vfield(mc::stype::float4, mc::vtype::vertex, { 0 })));
            buff.fields.push_back(mc::field::ptr(new mc::vfield(mc::stype::float4, mc::vtype::vertex, { 1 })));
            buff.fields.push_back(mc::field::ptr(new mc::vfield(mc::stype::float4, mc::vtype::vertex, { 2 })));
            buff.fields.push_back(mc::field::ptr(new mc::cfield(mc::stype::int4, mc::anyType::getValue(mc::stype::int4, -20))));
            fu.buffers.push_back(buff);
            fu.buffers.push_back(buff);
            ci_c.file_units.push_back(fu);
            return ci_c;
        }, {"tests/acceptance/res/Cube_code.txt"}),

        Case([]() {
            mc::compilationInfo ci_c;
            mc::unit fu;
            fu.c = mc::ctype::per_mesh;
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::buffers_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::buffer_size)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::entries_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::entries_per_buffer)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::entry_size)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::fields_per_unit)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::fields_per_buffer)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::fields_per_entry)));
            fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::field_size)));
            mc::buffer buff;
            buff.c = mc::ctype::per_vertex;
            buff.preamble.push_back(mc::field::ptr(new mc::cfield(mc::stype::int4, mc::anyType::getValue(mc::stype::int4, -30))));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::buffer_size)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::entries_per_buffer)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::entry_size)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::fields_per_buffer)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::fields_per_entry)));
            buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::field_size)));
            buff.fields.push_back(mc::field::ptr(new mc::vfield(mc::stype::float4, mc::vtype::vertex, { 0 })));
            buff.fields.push_back(mc::field::ptr(new mc::vfield(mc::stype::float4, mc::vtype::vertex, { 1 })));
            buff.fields.push_back(mc::field::ptr(new mc::vfield(mc::stype::float4, mc::vtype::vertex, { 2 })));
            buff.fields.push_back(mc::field::ptr(new mc::cfield(mc::stype::int4, mc::anyType::getValue(mc::stype::int4, -20))));
            fu.buffers.push_back(buff);
            fu.buffers.push_back(buff);
            ci_c.units.insert({ "mesh", fu });

            mc::fileUnit fu1;
            fu1.output_file = "tests/acceptance/res/{mesh}_code_unit_references.txt";
            fu1.c = mc::ctype::per_mesh;
            fu1.mode = mc::printMode::plainText;
            fu1.preamble.push_back(mc::field::ptr(new mc::cfield(mc::stype::int4, mc::anyType::getValue(mc::stype::int4, -50))));
            fu1.preamble.push_back(mc::field::ptr(new mc::ufield("mesh")));
            ci_c.file_units.push_back(fu1);
            return ci_c;
        }, {"tests/acceptance/res/Cube_code_unit_references.txt"}),

        Case(Case::getFromFile("tests/acceptance/1format-u.json"), { "tests/acceptance/res/Cube_json_u.txt" }),
        Case(Case::getFromFile("tests/acceptance/1format-s.json"), { "tests/acceptance/res/Cube_json_s.txt" }),
        Case(Case::getFromFile("tests/acceptance/1format-su.json"), { "tests/acceptance/res/Cube_json_su.txt" }),

        Case(Case::getFromFile("tests/acceptance/1format.format"), { "tests/acceptance/res/Cube_format.txt" }),
        Case(Case::getFromFile("tests/acceptance/1format-u.format"), { "tests/acceptance/res/Cube_format_u.txt" })


        }, { "tests/acceptance/1reference.txt" },
        "tests/acceptance/1cube.obj"
    );
    std::cout << "\n";
    std::cout << "test #2\n";
    run({
        Case(Case::getFromFile("tests/acceptance/2.format"), { "tests/acceptance/res/seg1w.mesh" })
        }, {"tests/acceptance/2reference.mesh"}, "tests/acceptance/2back_rooms.glb"
    );
    std::cout << "test #3\n";
    run({
        Case(Case::getFromFile("tests/acceptance/3.format"), {
            "tests/acceptance/res/cube.mesh", "tests/acceptance/res/capsule.mesh",
            "tests/acceptance/res/icosphere.mesh", "tests/acceptance/res/plane.mesh",
            "tests/acceptance/res/skybox.mesh", "tests/acceptance/res/uiQuad.mesh"})
        }, {
            "tests/acceptance/3cube.mesh", "tests/acceptance/3capsule.mesh",
            "tests/acceptance/3icosphere.mesh", "tests/acceptance/3plane.mesh",
            "tests/acceptance/3skybox.mesh", "tests/acceptance/3uiQuad.mesh"
        }, "tests/acceptance/3primitives.glb"
    );


    std::cout << "\n test #4\n";
    run({
        Case(Case::getFromFile("tests/acceptance/4.format"), {
            "tests/acceptance/res/4anim-test_base.animation",
            "tests/acceptance/res/4anim-test_wave.animation",
            "tests/acceptance/res/4anim-test_tantacle.mesh"})
        }, {
            "tests/acceptance/4ref_base.animation",
            "tests/acceptance/4ref_wave.animation",
            "tests/acceptance/4ref_tantacle.mesh"
        }, "tests/acceptance/4anim-test.glb"
    );
}
