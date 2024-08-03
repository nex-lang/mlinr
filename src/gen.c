#include "gen.h"

#include <string.h>

// #if defined(TARGET_X86)
//     #include "x86/main.h"
// #elif defined(TARGET_ARM)
//     #include "arm/main.h"
// #elif defined(TARGET_RISCV)
//     #include "riscv/main.h"
// #endif

Generator* gen_init(const char* filename) {
    Generator* gen = malloc(sizeof(Generator));
    if (!gen) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    #ifdef _WIN32
        char command[512];
        snprintf(command, sizeof(command), "type nul > \"%s\"", filename);
    #else
        char command[512];
        snprintf(command, sizeof(command), "touch \"%s\"", filename);
    #endif

    int result = system(command);
    if (result != 0) {
        perror("Failed to create file");
    } else {
        printf(": (%s)\n", filename);
    }


    gen->fp = fopen(filename, "w");
    if (!gen->fp) {
        perror("Error opening output file");
        free(gen);
        return NULL;
    }

    #if defined(TARGET_X86)
        WO(gen->fp, 0, "section .text\n");
        WO(gen->fp, 0, "global _start\n\n");
    #elif defined(TARGET_ARM)
        WO(gen->fp, 0, ".text\n");
        WO(gen->fp, 0, ".global _start\n\n");
    #elif defined(TARGET_RISCV)
        WO(gen->fp, 0, ".section .text\n");
        WO(gen->fp, 0, ".globl _start\n\n");
    #endif

    return gen;
}

void gen_free(Generator* gen) {
    fclose(gen->fp);
    free(gen);
}

void gen_stmt(AST_Instruction statement, Generator* gen) {
    switch (statement.type) {
        case INSTR_RETURN:
            #if defined(TARGET_X86)
                WO(gen->fp, 1, "mov rax, 0\n");
            #elif defined(TARGET_ARM)
                WO(gen->fp, 1, "mov r0, #0\n");
            #elif defined(TARGET_RISCV)
                WO(gen->fp, 1, "li a0, 0\n");
            #endif
            break;
        default:
            break;
    }
}

void generate_program(AST_Node* node, Generator* gen) {
    if (!node) return;

    switch (node->data.pinstruction.type) {
        case PINSTR_DEFINE:
            #if defined(TARGET_X86)
                WO(gen->fp, 0, "_start:\n");
            #elif defined(TARGET_ARM)
                WO(gen->fp, 0, "_start:\n");
            #elif defined(TARGET_RISCV)
                WO(gen->fp, 0, "_start:\n");
            #endif

            for (size_t i = 0; i < node->data.pinstruction.data.define.block.size; i++) {
                gen_stmt(node->data.pinstruction.data.define.block.instructions[i], gen);
            }

            #if defined(TARGET_X86)
                WO(gen->fp, 1, "mov rax, 60\n");
                WO(gen->fp, 1, "syscall\n");
            #elif defined(TARGET_ARM)
                WO(gen->fp, 1, "mov r7, #1\n");
                WO(gen->fp, 1, "swi 0\n");
            #elif defined(TARGET_RISCV)
                WO(gen->fp, 1, "li a7, 10\n");
                WO(gen->fp, 1, "ecall\n");
            #endif

            break;

        default:
            break;
    }

    generate_program(node->left, gen);
    generate_program(node->right, gen);
}

void generate(char* filename, char* arch) {
    int arch_macro = get_arch_macro(arch);
    if (arch_macro == -1) {
        fprintf(stderr, "Invalid architecture specified.\n");
        exit(EXIT_FAILURE);
    }
    
    #if arch_macro == TARGET_X86
        #define TARGET_X86
    #elif arch_macro == TARGET_ARM
        #define TARGET_ARM
    #elif arch_macro == TARGET_RISCV
        #define TARGET_RISCV
    #endif

    Parser* parser = parser_init(filename);

    parser_parse(parser);

    filename = get_tempname(filename);


    Generator* gen = gen_init(filename);
    generate_program(parser->root, gen);

    gen_free(gen);
}

int get_arch_macro(const char* arch) {
    if (strcmp(arch, "x86") == 0) {
        return TARGET_X86;
    } else if (strcmp(arch, "arm") == 0) {
        return TARGET_ARM;
    } else if (strcmp(arch, "riscv") == 0) {
        return TARGET_RISCV;
    } else {
        return -1; // Invalid architecture
    }
}