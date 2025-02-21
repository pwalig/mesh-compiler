#ifdef _DEBUG
#include "tests.h"
#include "tests/testing.h"
#endif
#include "core/runner.h"

int main(int argc, char** argv) {
#ifdef _DEBUG
    tests::run();
    //mainTest();
#endif // _DEBUG
    mc::run(argc, argv);

    return 0;
}