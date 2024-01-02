#include "cpu6502.h"

int main() {

    MEM mem;
    CPU cpu;

    cpu.reset(mem);

    // cpu.exec(2, mem);

    return 0;
}
