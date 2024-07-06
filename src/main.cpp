#include "meshCompiler.h"
#include "assimpReader.h"
#include "tests.h"

int main(int argc, char** argv) {
#ifdef tests_included
    mainTest();
#else
    mesh_compiler::compilationInfo ci;
    ci.output_file = "test/out";
    ci.format_file = "test/.format";
    ci.debug_messages = true;
    assimp::readFile("test/test.obj", std::bind(mesh_compiler::compileScene, std::placeholders::_1, ci));
#endif // tests_included

    return 0;
}