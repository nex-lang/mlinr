#ifndef ARTHLOGIC_H
#define ARTHLOGIC_H

#include "ast.h"

#include "x86/stack.h"

char* get_movasm(X86Stack* stack, AST_Operand* op);
char* get_refasm(X86Stack* stack, AST_Operand* op);
char* get_refasmid(X86Stack* stack, int32_t id);
char* get_movasmid(X86Stack* stack, int32_t id);

#endif // ARTHLOGIC_H