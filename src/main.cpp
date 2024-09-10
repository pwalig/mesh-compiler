#include "meshCompiler.h"
#include "assimpReader.h"
#ifdef _DEBUG
#include "tests.h"
#include "unit_testing.h"
#endif

int main(int argc, char** argv) {
#ifdef tests_included
    unit_testing::run();
    mainTest();
#else
    mesh_compiler::run(argc, argv);
#endif // tests_included

    return 0;
}