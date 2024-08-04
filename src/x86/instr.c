#include "x86/instr.h"

void X86_instr(AST_Instruction statement, Generator* gen) {
    switch (statement.type) {
        case INSTR_ASSGN:
            break;
        case INSTR_ALLOCA:
            break;
        default:
            break;
    }
    
}

void X86_alloca(AST_Instruction statement, Generator* gen) {
    if (statement.data.alloca.type == ALLOCA_VAL) {
        WO(gen->fp, 1, "alloca %d", statement.data.alloca.sg.)
    }
}