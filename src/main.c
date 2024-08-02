#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Invalid use! Propperly use: mlinr <x86|arm|riscv> <filename>\n");
        return 1;
    }    

    if (strcmp(argv[1], "x86") == 0) {
        #define TARGET_X86
    } else if (strcmp(argv[1], "arm") == 0) {
        #define TARGET_ARM
    } else if (strcmp(argv[1], "riscv") == 0) {
        #define TARGET_RISCV
    } else {
        exit(EXIT_FAILURE);
    }


    Parser* parser = parser_init(argv[2]);

    parser_parse(parser);


    return 0;
}