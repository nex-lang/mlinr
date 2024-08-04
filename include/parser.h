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

    uint64_t scope;
} Parser;

#define ES(parser) ((parser)->scope += 1);

Parser* parser_init(char* filename);
void parser_free(Parser* parser);

bool parser_expect(Parser* parser, uint8_t expected);
void parser_consume(Parser* parser);

void parser_parse(Parser* parser);

AST_Block parser_parse_instructions(Parser* parser);
AST_Node* parser_parse_pinstruction(Parser* parser);

AST_Literal parser_parse_literal(Parser* parser);
AST_Operand parser_parse_op(Parser* parser);


AST_Instruction parser_parse_instruction(Parser* parser);
InstrAlloca parser_parse_alloca(Parser* parser);
InstrBinOp parser_parse_binop(Parser* parser);
InstrAssign parser_parse_assgn(Parser* parser);
InstrReturn parser_parse_ret(Parser* parser);

PrimInstrDefine parser_parse_define(Parser* parser);

size_t type_to_size(unsigned int type);
uint64_t parser_parse_ulit(Parser* parser, char* use);

void symtbl_insert(Parser* parser, Symbol* symbol, char* raw_symb);

#endif // PARSER_H