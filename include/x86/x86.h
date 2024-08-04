#ifndef X86_X86_H
#define X86_X86_H

#include "ast.h"
#include "gen.h"

#include "x86/stack.h"
#include "x86/instr.h"

void x86(AST_Node* node, Generator* gen);

#endif // X86_X86_H