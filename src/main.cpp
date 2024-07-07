#include "meshCompiler.h"
#include "assimpReader.h"
//#include "tests.h"

int main(int argc, char** argv) {
#ifdef tests_included
    mainTest();
#else
    mesh_compiler::run(argc, argv);
#endif // tests_included

    return 0;
}