#ifndef X86_MEM_H
#define X86_MEM_H


#define RAX 0
#define RCX 1
#define RBX 2
#define RDX 3
#define R8 4
#define R9 5
#define R10 6
#define R11 7

#include "x86/stack.h"

#include "ast.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

char* get_movasm(X86Stack* stack, AST_Operand* op);
char* get_refasm(X86Stack* stack, AST_Operand* operand, char** reg, uint8_t op);
void promote_register(FILE* fp, int op1, int op2, char** reg1, char** reg2, bool sign);
char* get_refasmid(X86Stack* stack, int32_t id, bool ekw);
char* get_movasmid(X86Stack* stack, int32_t id);
char* get_kwsz(size_t sz);


#endif // X86_MEM_H