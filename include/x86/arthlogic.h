#ifndef ARTHLOGIC_H
#define ARTHLOGIC_H

#include "ast.h"

#include "x86/stack.h"

char* get_movasm(X86Stack* stack, AST_Operand* op);
char* get_refasm(X86Stack* stack, AST_Operand* op);

#endif // ARTHLOGIC_H