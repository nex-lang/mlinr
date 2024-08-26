#include "x86/x86.h"

#include "gen.h"

#include <string.h>

void x86(AST_Node* node, FILE* fp, X86Stack* stack, HexTable* hex) {
    if (node == NULL) {
        return;
    }
    bool mep = false;

    switch (node->data.pinstruction.type) {
        case PINSTR_DEFINE:
            WO(fp, 0, "%s:\n", node->data.pinstruction.data.define.id);

            if (node->data.pinstruction.data.define.id[0] == 'x') {
                int32_t id = hextbl_exhex(node->data.pinstruction.data.define.id);
                hextbl_insert(hex, hymbol_init(id, HYMBOL_MEP));
            } else if (strcmp(node->data.pinstruction.data.define.id, "_start") == 0) {
                mep = true; // bootleg implementation of MEP
                hextbl_insert(hex, hymbol_init(0, HYMBOL_MEP));
            } else {
                return;
            }

            x86_define_sz(stack, node->data.pinstruction.data.define);
            x86_define_param(stack, node->data.pinstruction.data.define);

            WO(fp, 1, "mov rbp, rsp\n");
            WO(fp, 1, "and rbp, -16\n");   
            WO(fp, 1, "sub rsp, %lu\n\n", stack->size); 
            stack->off = stack->size;          

            for (size_t i = 0; i < node->data.pinstruction.data.define.block.size; i++) {
                X86_instr(node->data.pinstruction.data.define.block.instructions[i], fp, stack, mep);
            }

            break;
        default:
            break;
    }

    x86(node->right, fp, stack, hex);
}