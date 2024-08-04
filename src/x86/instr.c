#include "x86/instr.h"

#include "io.h"

void X86_instr(AST_Instruction statement, X86Stack* stack) {
    switch (statement.type) {
        case INSTR_RETURN:
            break;
        case INSTR_ASSGN:
            break;
        case INSTR_ALLOCA:
            break;
        default:
            break;
    }
    
}

void X86_return(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    // ! KW MOVING / HANDLED IN MEM.C
    WO(fp, 1, "mov rax, %lu", statement.data.ret.val.value.literal.value.uint);
    WO(fp, 1, "ret");
}

void X86_alloca(AST_Instruction statement, X86Stack* stack) {
    if (statement.data.alloca.type == ALLOCA_VAL) {
        
    
    }
}