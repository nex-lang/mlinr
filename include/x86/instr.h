#ifndef X86_INSTR_H
#define X86_INSTR_H

#include "ast.h"

#include "x86/stack.h"

#include <stdio.h>

void X86_instr(AST_Instruction statement, FILE* fp, X86Stack* stack);

void X86_return(AST_Instruction statement, FILE* fp, X86Stack* stack);
void X86_alloca(AST_Instruction statement, FILE* fp, X86Stack* stack);
void X86_assgn(AST_Instruction statement, FILE* fp, X86Stack* stack);
void x86_binop(AST_Instruction statement, FILE* fp, X86Stack* stack);


#endif // X86_INSTR_H