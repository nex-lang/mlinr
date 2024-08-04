#ifndef X86_INSTR_H
#define X86_INSTR_H

#include "ast.h"
#include "gen.h"

void X86_instr(AST_Instruction statement, Generator* gen);

#endif // X86_INSTR_H