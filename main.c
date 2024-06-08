#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include "base.h"

// 8086 mental model:
// - mov data int registers
// - do something
// - move memory back
//
// ^^ before we get to that we have to understand what instructions the CPU can execute
// 
// 1: `MOV` data from one register to another
//                                                   AX = BX (variable assignment)
//      - copy bx -> ax `mov ax,bx` -- INTEL SYNTAX: AX (dest), BX(source)
//      - running `mov ax,bx` through an assembler will encode the 
//
//      // bit pattern for instruction
//                           2     3     3
//                           MOD - REG - R/M
//      [1 0 0 0 1 0 D W] [- - - - - - - -]
//      6 bits == OP encoding :: D = (0|1) W = (0|1)
//
//      MOD     - tells if memory operation or register operation (2 bits)
//                  - if 11 reg - r/m encode registers
//      REG     - encodes register (3 bits)
//      REG/MEM - encodes registers based on mod values (3 bits)
//      D       - if 0 -- means REG is not dest -- if 1 means REG is dest
//      W       - is it 16 bits or not (0 = 8 bits - 1 16 bits)
//
//      `mov ax, bx` = move complete 16 bits from bx to 16 bits in ax
//      `mov al, bl` = move 8 bits from bl to 6 bits in al (low bits)
//      `mov ah, bh` = move 8 bits from bh to 6 bits in ah (high bits)

// MOD = the op code
// if direction == 0
//  then REG is the source register && REG/MEM is the destination
// else
//  then REG IS the destination register && REG/MEM is the source

// op encoding
#define MOD_MEM_MODE        0b00
#define MOD_8_BIT_MODE      0b01
#define MOD_16_BIT_MODE     0b10
#define MOD_REGISTER_MODE   0b01

typedef struct {
    u8 bits;
    u8 w_table[2][3];
} Register;

typedef struct {
    u8 opp;
    u8 d;
    u8 w;
    u8 mod;
    u8 regrm[2];
} Instruction;

typedef struct {
    u8 bits;
    u8 str[4];
} Op;


Register registers[] = {
    {0b000, {"al", "ax"}},
    {0b001, {"cl", "cx"}},
    {0b010, {"dl", "dx"}},
    {0b011, {"bl", "bx"}},
    {0b100, {"ah", "sp"}},
    {0b101, {"ch", "bp"}},
    {0b110, {"dh", "si"}},
    {0b111, {"bh", "di"}}
};

Op ops[] = {
    {0b100010, "mov"},
};

int main() {
    String buf = read_file(U8("./listing_38_many_register_mov"), U8("rb"));

    printf("bits 16\n\n");

    for (int i = 0; i < buf.length; i+=2) {
        Instruction inst;
        {
            u8 *mem = &buf.str[i];
            inst.opp = (*mem & 0b11111100) >> 2;
            inst.d = (*mem & 0b00000010) >> 1;
            inst.w = (*mem & 0b00000001) >> 0;
        }
        {
            u8 *mem = &buf.str[i+1];
            inst.mod = (*mem & 0b1100000)>>6;
            inst.regrm[0] = (*mem & 0b00111000)>>3;
            inst.regrm[1] = (*mem & 0b00000111)>>0;

        }

        for(int i = 0; i < sizeof(ops) / sizeof(Op); i++) {
            if (ops[i].bits == inst.opp) {
                printf("%s ", ops[i].str);
            }
        }

        // isnt.d == 0 rm first, reg second
        // inst.d == 1 reg first, rm second
        if (inst.d == 0x1) {
            printf("%s", registers[inst.regrm[0]].w_table[inst.w]);
            printf(", ");
            printf("%s\n", registers[inst.regrm[1]].w_table[inst.w]);
        } else {
            printf("%s", registers[inst.regrm[1]].w_table[inst.w]);
            printf(", ");
            printf("%s\n", registers[inst.regrm[0]].w_table[inst.w]);
        }

    }

    return 0;
}
