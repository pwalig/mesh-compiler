#include "meshCompiler.h"
#include "assimpReader.h"
#ifdef _DEBUG
#include "tests.h"
#include "unit_testing.h"
#endif

int main(int argc, char** argv) {
#ifdef _DEBUG
    unit_testing::run();
    mainTest();
#else
    mesh_compiler::run(argc, argv);
#endif // _DEBUG

    return 0;
}