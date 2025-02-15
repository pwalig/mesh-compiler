#ifdef _DEBUG
#include "tests.h"
#include "tests/testing.h"
#endif
#include "core/runner.h"

int main(int argc, char** argv) {
#ifdef _DEBUG
    tests::run();
    mainTest();
    mc::run(argc, argv);
#else
    mc::run(argc, argv);
#endif // _DEBUG

    return 0;
}