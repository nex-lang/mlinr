#ifndef X86_X86_H
#define X86_X86_H

#include "x86/register.h"
#include "x86/stack.h"
#include "x86/instr.h"

#include "hextbl.h"

#include <stdio.h>

void x86(AST_Node* node, FILE* fp, X86Stack* stack, HexTable* hex);

#endif // X86_X86_H