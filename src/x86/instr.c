#include "x86/instr.h"
#include "x86/arthlogic.h"

#include "token.h"
#include "io.h"

void X86_instr(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    switch (statement.type) {
        case INSTR_RETURN:
            X86_return(statement, fp, stack);
            break;
        case INSTR_ASSGN:
            X86_assgn(statement, fp, stack);
            break;
        case INSTR_ALLOCA:
            break;
        case INSTR_BINARY_OP:
            x86_binop(statement, fp, stack);
            break;
        default:
            break;
    }
    
}

void X86_return(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    // ! KW MOVING / HANDLED IN MEM.C
    WO(fp, 1, "add rsp, %lu\n", stack->size);  
    WO(fp, 1, "mov rdi, %lu\n", statement.data.ret.val.value.literal.value.uint);
    WO(fp, 1, "ret\n");
}

void X86_alloca(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    if (statement.data.alloca.type == ALLOCA_VAL) {
        
    
    }
}

void X86_assgn(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    x86_push(stack, statement.data.assgn.instr->data.alloca.off, statement.data.assgn.iden, statement.data.assgn.instr->data.alloca.size);
}

void x86_binop(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    InstrBinOp instr = statement.data.bin;
    char* movasm = NULL;
    char* refasm = NULL;

    refasm = get_refasm(stack, &statement.data.bin.o1);
    if (refasm == NULL) {
        return;
    }

    movasm = get_movasm(stack, &statement.data.bin.o1);
    WO(fp, 1, "%s %s, rax\n", movasm, refasm);

    refasm = get_refasm(stack, &statement.data.bin.o2);
    if (refasm == NULL) {
        free(refasm);
        return;
    }

    movasm = get_movasm(stack, &statement.data.bin.o2);
    WO(fp, 1, "%s %s, rbx\n", movasm, refasm);

    switch (instr.type) {
        case TOK_ADD:
            WO(fp, 1, "add rax, rbx\n");
            break;
        case TOK_SUB:
            WO(fp, 1, "sub rax, rbx\n");
            break;
        case TOK_MUL:
            WO(fp, 1, "imul rax, rbx\n");
            break;
        case TOK_SDIV:
            WO(fp, 1, "cqo\n");
            WO(fp, 1, "idiv rbx\n");
            break;
        case TOK_UDIV:
            WO(fp, 1, "mov edx, 0\n");
            WO(fp, 1, "div rbx\n");
            break;
        case TOK_AND:
            WO(fp, 1, "and rax, rbx\n");
            break;
        case TOK_OR:
            WO(fp, 1, "or rax, rbx\n");
            break;
        case TOK_XOR:
            WO(fp, 1, "xor rax, rbx\n");
            break;
        case TOK_SHL:
            WO(fp, 1, "shl rax, cl\n");
            break;
        case TOK_SHR:
            WO(fp, 1, "shr rax, cl\n");
            break;
        case TOK_EQ:
            WO(fp, 1, "cmp rax, rbx\n");
            WO(fp, 1, "sete al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_NE:
            WO(fp, 1, "cmp rax, rbx\n");
            WO(fp, 1, "setne al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_GT:
            WO(fp, 1, "cmp rax, rbx\n");
            WO(fp, 1, "setg al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_GE:
            WO(fp, 1, "cmp rax, rbx\n");
            WO(fp, 1, "setge al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_LT:
            WO(fp, 1, "cmp rax, rbx\n");
            WO(fp, 1, "setl al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_LE:
            WO(fp, 1, "cmp rax, rbx\n");
            WO(fp, 1, "setle al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_fEQ:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "sete al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_fNE:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setne al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_fGT:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setg al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_fGE:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setge al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_fLT:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setl al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        case TOK_fLE:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setle al\n");
            WO(fp, 1, "movzx rax, al\n");
            break;
        default:
            break;
    }
}
