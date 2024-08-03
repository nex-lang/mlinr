#include "gen.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Invalid use! Propperly use: mlinr <x86|arm|riscv> <filename>\n");
        return 1;
    }    

    generate(argv[2], argv[1]);
    
    return 0;
}