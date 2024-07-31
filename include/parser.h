#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "symtbl.h"

typedef struct Parser {
    Lexer* lexer;
    Token* cur;

    AST_Node* tree;
    AST_Node* root;
    SymTable* tbl;

    uint8_t nest;
} Parser;

Parser* parser_init(char* filename);
void parser_free(Parser* parser);

bool parser_expect(Parser* parser, uint8_t expected);
void parser_consume(Parser* parser);

void parser_parse(Parser* parser);

AST_Node* parser_parse_define(Parser* parser);

#endif // PARSER_H