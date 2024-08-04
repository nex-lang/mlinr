#include "gen.h"

#include <string.h>

#if defined(TARGET_X86)
    #include "x86/x86.h"
#elif defined(TARGET_ARM)
    #include "arm/arm.h"
#elif defined(TARGET_RISCV)
    #include "riscv/riscv.h"
#endif

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

void generate_program(AST_Node* node, Generator* gen) {
    if (!node) return;

    #if defined(TARGET_X86)
        x86(node, gen);
    #elif defined(TARGET_ARM)
        arm(gen->fp, 0, "_start:\n");
    #elif defined(TARGET_RISCV)
        riscv(gen->fp, 0, "_start:\n");
    #endif
}

void generate(char* filename, char* arch) {
    if (strcmp(arch, "x86") == 0) {
        #define TARGET_X86
    } else if (strcmp(arch, "arm") == 0) {
        #define TARGET_ARM
    } else if (strcmp(arch, "riscv") == 0) {
        #define TARGET_RISCV
    } else {
        exit(EXIT_FAILURE);
    }

    Parser* parser = parser_init(filename);

    parser_parse(parser);

    filename = get_tempname(filename);


    Generator* gen = gen_init(filename);
    generate_program(parser->root, gen);

    gen_free(gen);
}