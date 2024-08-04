#include "x86/x86.h"

#include "gen.h"

void x86(AST_Node* node, Generator* gen) {
    if (!node) return;

    switch (node->data.pinstruction.type) {
        case PINSTR_DEFINE:
            WO(gen->fp, 0, "_start:\n");

            for (size_t i = 0; i < node->data.pinstruction.data.define.block.size; i++) {
                X86_instr(node->data.pinstruction.data.define.block.instructions[i], gen);
            }

            WO(gen->fp, 1, "mov rax, 60\n");
            WO(gen->fp, 1, "syscall\n");

            break;
        default:
            break;
    }

    x86(node->left, gen);
    x86(node->right, gen);
}