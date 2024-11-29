#ifndef GEN_H
#define GEN_H

#define TARGET_X86 0
#define TARGET_ARM 0
#define TARGET_RISCV 0

#include "parser.h"
#include "hextbl.h"

#if defined(TARGET_X86)
    #include "x86/x86.h"
#elif defined(TARGET_ARM)
    #include "arm/arm.h"
#elif defined(TARGET_RISCV)
    #include "riscv/riscv.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    FILE* fp;
    union {
        X86Stack* x86;
    } stack;
    HexTable* hex;
} Generator;

Generator* gen_init(const char* filename);
void gen_free(Generator* gen);

void gen_stmt(AST_Instruction statement, Generator* gen);
void generate_program(AST_Node* node, Generator* gen);

void gen_x86libs();
void generate(char* filename, char* arch);


#endif // GEN_H
