#include "parser.h"

Parser* parser_init(char* filename) {
    Parser* parser = calloc(1, sizeof(Parser));

    parser->lexer = lexer_init(filename);
    parser->cur = lexer_next_token(parser->lexer);
    parser->tbl = symtbl_init();
    parser->root = parser->tree;

    parser->nest = 0;

    return (Parser*)parser;
}

void parser_free(Parser* parser) {
    /*
    De-initializes provided parser
    */

    if (!parser) {
        return;
    }

    lexer_free(parser->lexer);
    token_free(parser->cur);

    ast_free(parser->tree);
    ast_free(parser->root);

    symtbl_free(parser->tbl);
    parser = NULL;
}


bool parser_expect(Parser* parser, uint8_t expected) {
    if (parser->cur->type != expected) {
        return false;            
    }

    parser_consume(parser);
    return true;
}


void parser_consume(Parser* parser) {
    if (parser->cur->type == TOK_EOF) {
        exit(0);
    }


    parser->cur = lexer_next_token(parser->lexer);
}