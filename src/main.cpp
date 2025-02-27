#ifdef _DEBUG
#include "tests.h"
#include "tests/testing.h"
#endif
#include "core/runner.h"
#include "endian.h"

int main(int argc, char** argv) {
    endian::init();
#ifdef _DEBUG
    tests::run();
    //mainTest();
#endif // _DEBUG
    mc::run(argc, argv);

    return 0;
}