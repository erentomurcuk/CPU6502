#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// https://www.youtube.com/watch?v=qJgsuQoy9bc&t=776s

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct MEM {
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];
    
    void init() {
        for (unsigned char & i : Data) {
            i = 0x0;
        }
    }
};

struct CPU {
    Word PC; // Program Counter
    Word SP; // Stack Pointer

    Byte A, X, Y; // Registers

    Byte C : 1; // Status Flag - Carry
    Byte Z : 1; // Status Flag - Zero
    Byte I : 1; // Status Flag - Interrupt Disable
    Byte D : 1; // Status Flag - Decimal Mode
    Byte B : 1; // Status Flag - Break Command
    Byte V : 1; // Status Flag - Overflow
    Byte N : 1; // Status Flag - Negative

    void reset(MEM &memory) { // Inaccurate
        PC = 0xFFFC;
        SP = 0x0100; // Normally it is supposed to be 0x00FF
        C = Z = I = D = B = V = N = 0x0;
        A = X = Y = 0x0;

        memory.init();
        // Set IO vectors
    }

    void exec(u32 cycles, MEM &memory) {

    }

};

int main() {

    MEM mem;
    CPU cpu;

    cpu.reset(mem);

    cpu.exec(2, mem);

    return 0;
}
