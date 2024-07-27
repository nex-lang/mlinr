#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("ERROR: NO INPUT FILE SPECIFIED");
        return 1;
    }    

    Lexer* lexer = lexer_init(argv[1]);


    Token* tok = lexer_next_token(lexer);

    while (1) {
        printf("[%d:%d] %s of %d\n", tok->col, tok->line, tok->value, tok->type);
        tok = lexer_next_token(lexer);

        if (tok->type == TOK_EOF) { break; }
    }


    free(lexer);

    return 0;
}