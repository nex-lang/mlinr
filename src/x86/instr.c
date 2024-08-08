#include "x86/instr.h"
#include "x86/arthlogic.h"
#include "x86/mem.h"


#include "token.h"
#include "io.h"

void X86_instr(AST_Instruction statement, FILE* fp, X86Stack* stack, bool mep) {
    switch (statement.type) {
        case INSTR_RETURN:
            if (mep) {
                WO(fp, 1, "\n\tmov rax, 60\n");
                WO(fp, 1, "syscall\n");
                break;
            }
            X86_return(statement, fp, stack);
            break;
        case INSTR_ASSGN:
            X86_assgn(statement, fp, stack);
            break;
        case INSTR_ALLOCA:
            break;
        case INSTR_STORE:
            x86_store(statement, fp, stack);
            break;
        case INSTR_BINARY_OP:
            x86_binop(statement, fp, stack, NULL);
            break;
        default:
            break;
    }
    
}

void X86_return(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    // ! KW MOVING / HANDLED IN MEM.C
    WO(fp, 1, "\n\tadd rsp, %lu\n", stack->size);  
    WO(fp, 1, "ret\n");
}

void X86_alloca(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    if (statement.data.alloca.type == ALLOCA_VAL) {
        
    
    }
}

void X86_assgn(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    if (statement.data.assgn.instr->type == INSTR_ALLOCA) {
        x86_push(stack, statement.data.assgn.instr->data.alloca.off, statement.data.assgn.iden, statement.data.assgn.instr->data.alloca.size);
    } else if (statement.data.assgn.instr->type == INSTR_BINARY_OP) {
        char* reg = (char*)malloc(16);
        x86_binop(*statement.data.assgn.instr, fp, stack, &reg);
        uint64_t n = x86_lookup_offset(stack, statement.data.assgn.iden);
        if (n > 0) {
            WO(fp, 1, "mov [rsp + %ld], %s\n", n, reg);
            return;
        }

        WO(fp, 1, "mov rax, [rsp]\n");
    } else if (statement.data.assgn.instr->type == INSTR_LOAD) {
        x86_load(*statement.data.assgn.instr, fp, stack);
    }
}


void x86_binop(AST_Instruction statement, FILE* fp, X86Stack* stack, char** reg) {
    InstrBinOp instr = statement.data.bin;
    char* movasm = NULL;
    char* refasm1 = NULL;
    char* refasm2 = NULL;
    char* reg1 = (char*)malloc(16);
    char* reg2 = (char*)malloc(16);
    bool sign = (instr.type == TOK_SDIV || instr.type == TOK_MUL || instr.type == TOK_SUB || instr.type == TOK_ADD);

    refasm1 = get_refasm(stack, &instr.o1, &reg1, RAX);
    if (refasm1 == NULL) return;

    printf("\n%s\n", reg1);
    
    movasm = get_movasm(stack, &instr.o1);
    WO(fp, 1, "%s %s, %s\n", movasm, reg1, refasm1);

    refasm2 = get_refasm(stack, &instr.o2, &reg2, RCX);
    if (refasm2 == NULL) return;

    movasm = get_movasm(stack, &instr.o2);
    WO(fp, 1, "%s %s, %s\n", movasm, reg2, refasm2);


    promote_register(fp, RAX, RCX, &reg1, &reg2, sign);

    switch (instr.type) {
        case TOK_ADD:
            WO(fp, 1, "add %s, %s\n", reg1, reg2);
            break;
        case TOK_SUB:
            WO(fp, 1, "sub %s, %s\n", reg1, reg2);
            break;
        case TOK_MUL:
            WO(fp, 1, "imul %s, %s\n", reg1, reg2);
            break;
        case TOK_SDIV:
            WO(fp, 1, "cqo\n");
            WO(fp, 1, "idiv %s\n", reg2);
            break;
        case TOK_UDIV:
            WO(fp, 1, "mov edx, 0\n");
            WO(fp, 1, "div %s\n", reg2);
            break;
        case TOK_AND:
            WO(fp, 1, "and %s, %s\n", reg1, reg2);
            break;
        case TOK_OR:
            WO(fp, 1, "or %s, %s\n", reg1, reg2);
            break;
        case TOK_XOR:
            WO(fp, 1, "xor %s, %s\n", reg1, reg2);
            break;
        case TOK_SHL:
            WO(fp, 1, "shl %s, cl\n", reg1);
            break;
        case TOK_SHR:
            WO(fp, 1, "shr %s, cl\n", reg1);
            break;
        case TOK_EQ:
            WO(fp, 1, "cmp %s, %s\n", reg1, reg2);
            WO(fp, 1, "sete al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_NE:
            WO(fp, 1, "cmp %s, %s\n", reg1, reg2);
            WO(fp, 1, "setne al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_GT:
            WO(fp, 1, "cmp %s, %s\n", reg1, reg2);
            WO(fp, 1, "setg al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_GE:
            WO(fp, 1, "cmp %s, %s\n", reg1, reg2);
            WO(fp, 1, "setge al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_LT:
            WO(fp, 1, "cmp %s, %s\n", reg1, reg2);
            WO(fp, 1, "setl al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_LE:
            WO(fp, 1, "cmp %s, %s\n", reg1, reg2);
            WO(fp, 1, "setle al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_fEQ:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "sete al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_fNE:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setne al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_fGT:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setg al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_fGE:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setge al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_fLT:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setl al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        case TOK_fLE:
            WO(fp, 1, "comiss xmm0, xmm1\n");
            WO(fp, 1, "setle al\n");
            WO(fp, 1, "movzx %s, al\n", reg1);
            break;
        default:
            break;
    }

    if (reg != NULL) {
        *reg = reg1;
    }
}


void x86_store(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    InstrStore instr = statement.data.store;

    char* reg1 = (char*)malloc(16);
    char* movasm = get_movasm(stack, &instr.push_op); 
    char* refasm = get_refasm(stack, &instr.push_op, &reg1, RAX);
    char* ptr_refasm = get_refasmid(stack, instr.ptr_op);

    if (movasm == NULL || refasm == NULL || ptr_refasm == NULL) {
        free(movasm);
        free(refasm);
        free(ptr_refasm);
        return;
    }

    WO(fp, 1, "%s rax, %s\n", movasm, refasm);

    WO(fp, 1, "mov %s, rax\n", ptr_refasm);
}

void x86_load(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    InstrLoad instr = statement.data.load;

    char* movasm = get_movasmid(stack, instr.ptr_op); 
    char* refasm = get_refasmid(stack, instr.ptr_op);

    WO(fp, 1, "%s rax, %s\n", movasm, refasm);
}