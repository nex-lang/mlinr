#include "x86/instr.h"
#include "x86/arthlogic.h"
#include "x86/mem.h"

#include "token.h"
#include "io.h"
#include "parser.h"

#include <string.h>

void X86_instr(AST_Instruction statement, FILE* fp, X86Stack* stack, bool mep) {
    printf("[%d]state: %d\n", mep, statement.type);

    switch (statement.type) {
        case INSTR_RETURN:
            X86_return(statement, fp, stack, mep);
            break;
        case INSTR_ASSGN:
            X86_assgn(statement, fp, stack);
            break;
        case INSTR_ALLOCA:
            X86_alloca(&statement, fp, stack);
            break;
        case INSTR_STORE:
            x86_store(statement, fp, stack);
            break;
        case INSTR_BINARY_OP:
            x86_binop(statement, fp, stack, NULL);
            break;
        case INSTR_CALL:
            x86_call(statement, fp, stack);
            break;
        default:
            break;
    }
    
}

void X86_return(AST_Instruction statement, FILE* fp, X86Stack* stack, bool mep) {
    // ! KW MOVING / HANDLED IN MEM.C
    printf("-> %i\n", mep);

    if (mep) {
        WO(fp, 1, "\n\tmov rax, 60\n");
        // WO(fp, 1, "mov rdi, %s\n", statement.data.ret.val.value.literal.value.);
        WO(fp, 1, "syscall\n");
        return;
    }
    WO(fp, 1, "\n\tadd rsp, %lu\n", stack->size);  
    WO(fp, 1, "ret\n");
}

void X86_alloca(AST_Instruction* statement, FILE* fp, X86Stack* stack) {
    if (statement->data.alloca.type == ALLOCA_VAL) {
        statement->data.alloca.size = type_to_size(statement->data.alloca.data.var);
    }
}

void X86_assgn(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    if (statement.data.assgn.instr->type == INSTR_ALLOCA) {
        X86_alloca(statement.data.assgn.instr, fp, stack);
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
    } else if (statement.data.assgn.instr->type == INSTR_CALL) {
        x86_call(*statement.data.assgn.instr, fp, stack);
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
    char* ptr_refasm = get_refasmid(stack, instr.ptr_op, false);

    if (movasm == NULL || refasm == NULL || ptr_refasm == NULL) {
        return;
    }

    WO(fp, 1, "%s rax, %s\n", movasm, refasm);

    WO(fp, 1, "mov %s, rax\n", ptr_refasm);
}

void x86_load(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    InstrLoad instr = statement.data.load;

    char* movasm = get_movasmid(stack, instr.ptr_op); 
    char* refasm = get_refasmid(stack, instr.ptr_op, true);

    WO(fp, 1, "%s rax, %s\n", movasm, refasm);
}

void x86_call(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    InstrCall instr = statement.data.call;
    char* type = ""; 

    if (instr.type != 0) {
        // type = smth
    }


    if (strcmp(instr.iden, "__malloc") == 0) {
        if (instr.args.size != 0) {
            return;
        }
        
        if (instr.args.args[0].type != OPERAND_LITERAL) {
            return;
        }


        if (!(instr.args.args[0].value.literal.type == TOK_L_SSUINT ||
            instr.args.args[0].value.literal.type == TOK_L_SUINT ||
            instr.args.args[0].value.literal.type == TOK_L_UINT ||
            instr.args.args[0].value.literal.type == TOK_L_LUINT)) {
            return;
        }

        WO(fp, 1, "mov rdi, %lu\n", instr.args.args[0].value.literal.value.uint);
        WO(fp, 1, "call __malloc\n");
    }
    // args
}

void x86_geptr(AST_Instruction statement, FILE* fp, X86Stack* stack) {
    /*
    getelementptr <type>, <type>* <ptrval>, <index1_type> <index1>, <index2_type> <index2>, ...
        1 e.g. %char_ptr = getelementptr inbounds [6 x i8], [6 x i8]* @my_string, i32 0
        2 e.g. %ptr = getelementptr inbounds [10 x [20 x i32]], [10 x [20 x i32]]* @nested_array, i32 0, i32 3, i32 5
            @nested_array = [ [i32, i32, i32, i32, i32, i32, ...], ... (10) ]

        note: use of u8 allows extended ascii (look into it)
    */


    InstrGEP instr;

    // if (instr.type == 0) {
        // WO(fp, 1, "call %s", instr.iden);
    // }
}


void x86_define_sz(X86Stack* stack, PrimInstrDefine instr) {
    for (size_t i = 0; i < instr.block.size; i++) {
        if (instr.block.instructions[i].type == INSTR_ALLOCA) {
            uint64_t sz = 0;
            switch (instr.block.instructions[i].data.alloca.type) {
                case ALLOCA_VAL:
                    sz = type_to_size(instr.block.instructions[i].data.alloca.data.var);
                    break;
                case ALLOCA_ALVAL:
                    sz = type_to_size(instr.block.instructions[i].data.alloca.data.alvar.type);
                    break;
                case ALLOCA_ARRAY:
                    sz = instr.block.instructions[i].data.alloca.data.array.val * type_to_size(instr.block.instructions[i].data.alloca.data.array.type);
                    break;
                case ALLOCA_ALARRAY:
                    sz = instr.block.instructions[i].data.alloca.data.alarray.val * type_to_size(instr.block.instructions[i].data.alloca.data.alarray.type);
                    break;
                default:
                    break;
            }
            stack->size += sz;
            instr.block.instructions[i].data.alloca.off = stack->size - sz;
            instr.block.instructions[i].data.alloca.size = sz;
        }

        else if (instr.block.instructions[i].type == INSTR_ASSGN) {
            uint64_t sz2 = 0;
            // FIXXXXXXX
            switch (instr.block.instructions[i].data.assgn.instr->data.alloca.type) {
                case ALLOCA_VAL:
                    sz2 = type_to_size(instr.block.instructions[i].data.assgn.instr->data.alloca.data.var);
                    break;
                case ALLOCA_ALVAL:
                    sz2 = type_to_size(instr.block.instructions[i].data.assgn.instr->data.alloca.data.alvar.type);
                    break;
                case ALLOCA_ARRAY:
                    sz2 = instr.block.instructions[i].data.assgn.instr->data.alloca.data.array.val * type_to_size(instr.block.instructions[i].data.assgn.instr->data.alloca.data.array.type);
                    break;
                case ALLOCA_ALARRAY:
                    sz2 = instr.block.instructions[i].data.assgn.instr->data.alloca.data.alarray.val * type_to_size(instr.block.instructions[i].data.assgn.instr->data.alloca.data.alarray.type);
                    break;
                default:
                    break;
            }

            stack->size += sz2;
            instr.block.instructions[i].data.alloca.off = stack->size - sz2;
            instr.block.instructions[i].data.alloca.size = sz2;

            if (instr.block.instructions[i].data.assgn.instr->type == INSTR_BINARY_OP) {
                stack->size += instr.block.instructions[i].data.assgn.instr->data.bin.size; 
            }
        }
    }
}


void x86_define_param(X86Stack* stack, PrimInstrDefine instr) {
    stack->arg_size = 0;
    if (instr.args.size != 0) {
        stack->arg_size = instr.args.size + 1;
    }

    stack->regargs = malloc(sizeof(int32_t) * instr.args.size);

    for (size_t i = 0; i < stack->arg_size; i++) {
        if (i > 5) {
            stack->size += instr.args.type[i];
            x86_push(stack, stack->size - instr.args.type[i], instr.args.id[i], instr.args.type[i]);
        } else {
            stack->regargs[i] = instr.args.id[i];
        }
    }

}