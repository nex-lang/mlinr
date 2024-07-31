#include "parser.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("ERROR: NO INPUT FILE SPECIFIED");
        return 1;
    }    


    Parser* parser = parser_init(argv[1]);

    parser_parse(parser);


    return 0;
}