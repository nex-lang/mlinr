#include "parser.h"

Parser* parser_init(char* filename) {
    /*
    Initializes the parser
    */

    Parser* parser = calloc(1, sizeof(Parser));

    parser->lexer = lexer_init(filename);
    parser->cur = lexer_next_token(parser->lexer);
    parser->tbl = symtbl_init();
    parser->tree = ast_init(ROOT);
    parser->root = parser->tree;

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

void parser_parse(Parser* parser) {
    while (parser->cur->type != TOK_EOF) {
        switch (parser->cur->type) {
        case TOK_DEFINE:
            parser->tree->right = parser_parse_define(parser);
            printf("NAME: %s\n", parser->tree->right->data.instruction.data.define.id);
            printf("TYPE: %d\n", parser->tree->right->data.instruction.data.define.type);
            break;        
        default:
            break;
        }

        if (!(parser->tree && parser->tree->right)) {
            break;
        }

        AST_Node* tmp = parser->tree->right; 

        parser->tree = tmp;
    }   
}

AST_Node* parser_parse_define(Parser* parser) {
    parser_consume(parser);

    InstrDefine instr;

    if (!parser_expect(parser, TOK_AT)) {
        REPORT_ERROR(parser->lexer, "E_SYMBOL_AF_DEFINEKW");
        return NULL;
    }

    if (parser->cur->type == TOK_IDEN) {
        instr.id = parser->cur->value;
        parser_consume(parser);
    }

    if (IS_TYPEKW(parser->cur->type)) {
        instr.type = parser->cur->type;
    } else {
        instr.type = 0;
    }

    parser_consume(parser);

    if (!parser_expect(parser, TOK_LPAREN)) {
        REPORT_ERROR(parser->lexer, "E_SYMBOL_PARAMS");
        return NULL;
    }

    instr.args.id = malloc(sizeof(char*));
    instr.args.type = malloc(sizeof(int));
    instr.args.size = 0;

    while (parser->cur->type != TOK_RPAREN) {
        if (parser->cur->type == TOK_PER) {
            parser_consume(parser);
            
            if (parser->cur->type != TOK_IDEN) {
                REPORT_ERROR(parser->lexer, "E_PARAM_ID");
            }
        }

        instr.args.id[instr.args.size] = parser->cur->value;
        parser_consume(parser);

        if (!parser_expect(parser, TOK_COLON)) {
            REPORT_ERROR(parser->lexer, "E_COLON_TO_SPEC_TYPE");
            return NULL;
        }

        instr.args.id[instr.args.size] = parser->cur->value;

        if (parser->cur->type == TOK_COMMA) {
            instr.args.size++;
            instr.args.id = realloc(instr.args.id, sizeof(char*) * instr.args.size);
            instr.args.type = realloc(instr.args.id, sizeof(int) * instr.args.size);
        }


    }

    parser_consume(parser); // consumes RPAREN

    AST_Node* n = ast_init(INSTRUCTION);
    n->data.instruction.data.define = instr;
    n->data.instruction.type = INSTR_DEFINE;

    return n;
}