#ifdef _DEBUG
#include "tests.h"
#include "tests/testing.h"
#endif
#include "core/runner.h"
#include "endian.h"

int main(int argc, char** argv) {
    init_host_endianness();
#ifdef _DEBUG
    tests::run();
    //mainTest();
#endif // _DEBUG
    mc::run(argc, argv);

    return 0;
}