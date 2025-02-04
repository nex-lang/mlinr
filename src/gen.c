#include "gen.h"

#include <string.h>

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

    gen->hex = hextbl_init();

    #if defined(TARGET_X86)

        gen->stack.x86 = x86_stack();
        WO(gen->fp, 0, "section .text\n");
        WO(gen->fp, 1, "global _start\n\n");
        // WO(gen->fp, 1, "extern __malloc\n"); 
        // WO(gen->fp, 1, "extern __free\n"); 
    #elif defined(TARGET_ARM)
        WO(gen->fp, 0, ".text\n");
        WO(gen->fp, 1, ".global _start\n\n");
    #elif defined(TARGET_RISCV)
        WO(gen->fp, 0, ".section .text\n");
        WO(gen->fp, 1, ".globl _start\n\n");
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
        x86(node, gen->fp, gen->stack.x86, gen->hex);
    #elif defined(TARGET_ARM)
        arm(gen->fp, 0, "_start:\n");
    #elif defined(TARGET_RISCV)
        riscv(gen->fp, 0, "_start:\n");
    #endif
}

void gen_x86libs() {
    EXEC("nasm -f elf64 src/x86/libs/stdmem.asm -o build/stdmem.o");
    EXEC("ar rcs build/libstdmem.a build/stdmem.o");
}

void generate(char* filename, char* arch) {
    if (strcmp(arch, "x86") == 0) {
        #define TARGET_X86 1
    } else if (strcmp(arch, "arm") == 0) {
        #define TARGET_ARM 1
    } else if (strcmp(arch, "riscv") == 0) {
        #define TARGET_RISCV 1
    } else {
        exit(EXIT_FAILURE);
    }

    Parser* parser = parser_init(filename);

    parser_parse(parser);

    filename = get_tempname(filename);
    Generator* gen = gen_init(filename);

    size_t len = strlen(filename);
    if (len < 4 || strcmp(filename + len - 4, ".asm") != 0) {
        fprintf(stderr, "Filename does not end with .asm\n");
        exit(EXIT_FAILURE);
    }

    char base[len - 3];
    strncpy(base, filename, len - 4);
    base[len - 4] = '\0'; 
    
    #if defined(TARGET_X86)
        x86(parser->root, gen->fp, gen->stack.x86, gen->hex);
        gen_x86libs();
        EXEC("nasm -f elf64 %s.asm -o %s.o", base, base);
        EXEC("ld -m elf_x86_64 -o %s %s.o", base, base);
    #elif defined(TARGET_ARM)
        arm(gen->fp, 0, "_start:\n");
    #elif defined(TARGET_RISCV)
        riscv(gen->fp, 0, "_start:\n");
    #endif

    parser_free(parser);
    gen_free(gen);
}