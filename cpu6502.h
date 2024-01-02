//
// Created by Eren Tomurcuk on 2.01.2024.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>

#ifndef CPU6502_CPU6502_H
#define CPU6502_CPU6502_H

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

    // Read 1 byte
    Byte operator[](u32 addr) const {
        return Data[addr]; // assert here Address is < MAX_MEM
    }

    // Write 1 byte
    Byte &operator[](u32 addr) {
        return Data[addr]; // assert here Address is < MAX_MEM
    }

    // Write 2 bytes
    void writeWord(u32 &cycles, Word value, u32 address) {
        Data[address]       = value & 0xFF;
        Data[address + 1]   = (value >> 8);
        cycles -= 2;
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
        PC                          = 0xFFFC;
        SP                          = 0x0100; // Normally it is supposed to be 0x00FF
        C = Z = I = D = B = V = N   = 0x0;
        A = X = Y                   = 0x0;

        memory.init();
        // Set IO vectors
    }

    Byte fetchByte(u32 &cycles, MEM &memory) {
        Byte data = memory[PC];
        PC++;
        cycles--;
        return data;
    }

    static Byte readByte(u32 &cycles, Word address, MEM &memory) {
        Byte data = memory[address];
        cycles--;
        return data;
    }

    static Word readWord(u32 &cycles, Word address, MEM &memory) {
        Byte lowByte = readByte(cycles, address, memory);
        Byte highByte = readByte(cycles, address + 1, memory);

        return lowByte | (highByte << 8);
    }

    Word fetchWord(u32 &cycles, MEM &memory) {
        // 6502 is little endian!
        Word data = memory[PC];
        PC++;

        data |= (memory[PC] << 8);
        PC++;

        cycles-=2;

        // to handle endianness swap bytes here as:
        /* if(PLATFORM_BIG_ENDIAN) swapBytesInWord(data); */

        return data;
    }

    /* OPCODES */

    static constexpr Byte
            INST_LDA_IM     = 0xA9,
            INST_LDA_ZP     = 0xA5,
            INST_LDA_ZPX    = 0xB5,
            INST_LDA_ABS    = 0xAD,
            INST_LDA_ABSX   = 0xBD,
            INST_LDA_ABSY   = 0xB9,
            INST_LDA_INDX   = 0xA1,
            INST_LDA_INDY   = 0xB1,
            INST_JSR        = 0x20;

    void LDASetStatus() {
        Z = (A==0);
        N = (A & 0b10000000) > 0;
    }

    void exec(u32 cycles, MEM &memory) {

        while (cycles > 0) {
            Byte inst = fetchByte(cycles, memory);

            switch (inst) {
                case INST_LDA_IM: {

                    Byte value = fetchByte(cycles, memory);
                    A = value;
                    LDASetStatus();

                } break;

                case INST_LDA_ZP: {

                    Byte zeroPageAddress = fetchByte(cycles, memory);
                    A = readByte(cycles, zeroPageAddress, memory);
                    LDASetStatus();

                } break;

                case INST_LDA_ZPX: {

                    Byte zeroPageAddress = fetchByte(cycles, memory);
                    zeroPageAddress += X;
                    cycles--;
                    A = readByte(cycles, zeroPageAddress, memory);
                    LDASetStatus();

                } break;

                case INST_LDA_ABS: {

                    Word absoluteAddress = fetchWord(cycles, memory);
                    A = readByte(cycles, absoluteAddress, memory);

                } break;

                case INST_LDA_ABSX: {

                    Word absoluteAddress = fetchWord(cycles, memory);
                    Word absoluteAddressX = absoluteAddress + X;
                    A = readByte(cycles, absoluteAddress, memory);

                    if ((absoluteAddressX - absoluteAddress) >= 0xFF ) cycles--;

                } break;

                case INST_LDA_ABSY: {

                    Word absoluteAddress = fetchWord(cycles, memory);
                    Word absoluteAddressY = absoluteAddress + Y;
                    A = readByte(cycles, absoluteAddress, memory);

                    if ((absoluteAddressY - absoluteAddress) >= 0xFF ) cycles--;

                } break;

                case INST_LDA_INDX: {

                    Byte ZeroPageAddress = fetchByte(cycles, memory);
                    ZeroPageAddress += X;
                    cycles--;
                    Word effectiveAddress = readWord(cycles, ZeroPageAddress, memory);
                    A = readByte(cycles, effectiveAddress, memory);

                } break;

                case INST_LDA_INDY: {

                    Byte ZeroPageAddress = fetchByte(cycles, memory);
                    Word effectiveAddress = readWord(cycles, ZeroPageAddress, memory);
                    Word effectiveAddressY = effectiveAddress + Y;
                    A = readByte(cycles, effectiveAddressY, memory);
                    if ((effectiveAddressY - effectiveAddress) >= 0xFF ) cycles--;

                } break;

                case INST_JSR: {
                    
                    Word subAddr = fetchWord(cycles, memory);
                    memory.writeWord(cycles,PC - 1, SP);
                    SP += 2;
                    PC = subAddr;
                    cycles--;

                } break;

                default: {

                    std::cerr << "Instruction not handled: " << inst << std::endl;

                } break;
            }
        }
    }
};

#endif //CPU6502_CPU6502_H
