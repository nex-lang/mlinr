#include "x86/x86.h"

#include "gen.h"

void x86(AST_Node* node, FILE* fp, X86Stack* stack) {
    if (node == NULL) {
        return;
    }

    switch (node->data.pinstruction.type) {
        case PINSTR_DEFINE:
            WO(fp, 0, "%s:\n", node->data.pinstruction.data.define.id);
 
            for (size_t i = 0; i < node->data.pinstruction.data.define.block.size; i++) {
                if (node->data.pinstruction.data.define.block.instructions[i].type == INSTR_ALLOCA) {
                    uint64_t sz = 0;
                    switch (node->data.pinstruction.data.define.block.instructions[i].data.alloca.type) {
                        case ALLOCA_VAL:
                            sz = type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.var);
                            break;
                        case ALLOCA_ALVAL:
                            sz = type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.alvar.type);
                            break;
                        case ALLOCA_ARRAY:
                            sz = node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.array.val * type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.array.type);
                            break;
                        case ALLOCA_ALARRAY:
                            sz = node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.alarray.val * type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.alarray.type);
                            break;
                        default:
                            break;
                    }
                    stack->size += sz;
                    node->data.pinstruction.data.define.block.instructions[i].data.alloca.off = stack->size - sz;
                    node->data.pinstruction.data.define.block.instructions[i].data.alloca.size = sz;
                }

                if (node->data.pinstruction.data.define.block.instructions[i].type == INSTR_ASSGN) {
                    uint64_t sz2 = 0;
                    switch (node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.alloca.type) {
                        case ALLOCA_VAL:
                            sz2 = type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.alloca.data.var);
                            break;
                        case ALLOCA_ALVAL:
                            sz2 = type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.alloca.data.alvar.type);
                            break;
                        case ALLOCA_ARRAY:
                            sz2 = node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.alloca.data.array.val * type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.alloca.data.array.type);
                            break;
                        case ALLOCA_ALARRAY:
                            sz2 = node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.alloca.data.alarray.val * type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.alloca.data.alarray.type);
                            break;
                        default:
                            break;
                    }
                    stack->size += sz2;
                    node->data.pinstruction.data.define.block.instructions[i].data.alloca.off = stack->size - sz2;
                    node->data.pinstruction.data.define.block.instructions[i].data.alloca.size = sz2;
                
                    if (node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->type == INSTR_BINARY_OP) {
                        stack->size += node->data.pinstruction.data.define.block.instructions[i].data.assgn.instr->data.bin.size; 
                    }

                }

                
            }

            WO(fp, 1, "mov rbp, rsp\n");
            WO(fp, 1, "and rbp, -16\n");   
            WO(fp, 1, "sub rsp, %lu\n\n", stack->size); 
            stack->off = stack->size;          

            for (size_t i = 0; i < node->data.pinstruction.data.define.block.size; i++) {
                X86_instr(node->data.pinstruction.data.define.block.instructions[i], fp, stack);
            }

            break;
        default:
            break;
    }

    x86(node->right, fp, stack);
}