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
                    switch (node->data.pinstruction.data.define.block.instructions[i].data.alloca.type) {
                        case ALLOCA_VAL:
                            stack->size += type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.var);
                            break;
                        case ALLOCA_ALVAL:
                            stack->size += type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.alvar.type);
                            break;
                        case ALLOCA_ARRAY:
                            stack->size += node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.array.val * type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.array.type);
                            break;
                        case ALLOCA_ALARRAY:
                            stack->size += node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.alarray.val * type_to_size(node->data.pinstruction.data.define.block.instructions[i].data.alloca.data.alarray.type);
                            break;
                        default:
                            break;
                    }
                }
            }

            WO(fp, 1, "mov rbp, rsp\n");
            WO(fp, 1, "and rbp, -16\n");   
            WO(fp, 1, "sub rsp, %lu\n\n", stack->size);           

            for (size_t i = 0; i < node->data.pinstruction.data.define.block.size; i++) {
                X86_instr(node->data.pinstruction.data.define.block.instructions[i], stack);
            }

            WO(fp, 1, "mov rax, 60\n");
            WO(fp, 1, "syscall\n");

            break;
        default:
            break;
    }

    x86(node->right, fp, stack);
}