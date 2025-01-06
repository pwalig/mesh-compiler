#include "meshCompiler.h"
#include "assimp-integration/reader.h"
#ifdef _DEBUG
#include "tests.h"
#include "unit_testing.h"
#include "tests/testing.h"
#endif

int main(int argc, char** argv) {
#ifdef _DEBUG
    unit_testing::run();
    tests::run();
    mainTest();
#else
    mesh_compiler::run(argc, argv);
#endif // _DEBUG

    return 0;
}