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
        parser_parse_instructions(parser);
        switch (parser->cur->type) {
        case TOK_DEFINE:
            
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


AST_Block parser_parse_instructions(Parser* parser) {
    AST_Block block;
    AST_Instruction instr;

    block.instructions = malloc(sizeof(AST_Instruction));
    block.size = 0;


    while (1) {
        if (parser->cur->type == TOK_RBRACK || parser->cur->type == TOK_EOF) {
            break;
        }
        
        instr = parser_parse_instruction(parser);

        block.instructions = realloc(block.instructions, (sizeof(AST_Instruction) * (block.size + 1)));

        if (block.instructions == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }

        block.instructions[block.size] = instr;
        
        block.size += 1;
    }

    parser_expect(parser, TOK_RBRACK);

    return block;
}


AST_Node* parser_parse_pinstruction(Parser* parser);


AST_Literal parser_parse_literal(Parser* parser) {
    AST_Literal lit;
    char *endptr;

    if (!IS_LITERAL(parser->cur->type)) {
        lit.type = -1;
        return lit;
    }

    lit.type = parser->cur->type;

    switch (lit.type) {
        case TOK_L_SSINT:
        case TOK_L_SINT:
        case TOK_L_INT:
        case TOK_L_LINT:
            lit.value.int_ = (int64_t)strtol(parser->cur->value, &endptr, 10);
            break;
        case TOK_L_SSUINT:
        case TOK_L_SUINT:
        case TOK_L_UINT:
        case TOK_L_LUINT:
            lit.value.uint = (uint64_t)strtoul(parser->cur->value, &endptr, 10);
            break;
        case TOK_L_FLOAT:
            lit.value.float_.bit32 = strtof(parser->cur->value, &endptr);
            break;
        case TOK_L_DOUBLE:
            lit.value.float_.bit64 = strtod(parser->cur->value, &endptr);
            break;
        case TOK_L_CHAR:
            lit.value.character = (parser->cur->value[0] != '\0') ? parser->cur->value[0] : '\0';
            break;
        case TOK_L_STRING:
            lit.value.string = parser->cur->value;
            break;
        default: lit.type = -1; return lit; break;
    }


    parser_consume(parser);
    return lit;
}

AST_Operand parser_parse_op(Parser* parser) {
    AST_Operand op = (AST_Operand){0};

    AST_Literal lit = parser_parse_literal(parser);
    if (lit.type != -1) {
        op.type = OPERAND_LITERAL;
        op.value.literal = lit;
        return op;
    }

    if (!parser_expect(parser, TOK_PER)) {
        return op;
    }

    if (parser->cur->type != TOK_IDEN) {
        return op; 
    }


    op.type = OPERAND_VARIABLE;
    op.value.variable = parser->cur->value;
    parser_consume(parser);

    return op;
}


AST_Instruction parser_parse_instruction(Parser* parser) {
    AST_Instruction instr;
    switch (parser->cur->type) {
    case TOK_ALLOCA:
        instr.type = INSTR_ALLOCA;
        instr.data.alloca = parser_parse_alloca(parser);
        break;
    default:
        instr.type = INSTR_BINARY_OP;
        instr.data.bin =  parser_parse_binop(parser);
        break;
    }

    return instr;
}


InstrAlloca parser_parse_alloca(Parser* parser) {
    /*
    alloca <element type>, <number of elements>
    alloca <mem> 
    */

    parser_consume(parser);

    InstrAlloca instr;

    if (IS_TYPEKW(parser->cur->type)) {
        instr.mult.unit = parser->cur->type;
        
        if (!parser_expect(parser, TOK_COMMA)) {
            REPORT_ERROR(parser->lexer, "E_COMMA_AF_TOFMEM");
            return instr;
        }

        instr.mult.n = parser_parse_op(parser);
        return instr;
    }

    instr.sg = parser_parse_op(parser);
    return instr;
}

InstrBinOp parser_parse_binop(Parser* parser) {
    /*
    <pre_op> op <result type> <o1>, <o2>
    */

    InstrBinOp instr;
    uint8_t cmp_prefix = 0;

    if (parser->cur->type == TOK_ICMP) {
        cmp_prefix = 1;
        parser_consume(parser);
    } else if (parser->cur->type == TOK_FCMP) {
        cmp_prefix = 2;
        parser_consume(parser);
    }

    if ((!IS_PBOP(parser->cur->type)) && cmp_prefix > 0) {
        REPORT_ERROR(parser->lexer, "E_CMP_AFTYPE");
        return instr;
    }

    if (!IS_PBOP(parser->cur->type)) {
        return instr;
    }

    if (cmp_prefix == 2) {
        switch (parser->cur->type) {
            case TOK_EQ:
                instr.type = TOK_fEQ;
                break;
            case TOK_NE:
                instr.type = TOK_fNE;
                break;
            case TOK_GT:
                instr.type = TOK_fGT;
                break;
            case TOK_GE:
                instr.type = TOK_fGE;
                break;
            case TOK_LT:
                instr.type = TOK_fLT;
                break;
            case TOK_LE:
                instr.type = TOK_fLE;
                break;
            default:
                break;
        }
    } else {
        instr.type = parser->cur->type;
    }

    parser_consume(parser);

    if (!IS_TYPEKW(parser->cur->type)) {
        REPORT_ERROR(parser->lexer, "E_TYPE_KW");
        return instr;
    }

    instr.size = parser->cur->type;
    parser_consume(parser);


    instr.o1 = parser_parse_op(parser);


    if (!parser_expect(parser, TOK_COMMA)) {
        REPORT_ERROR(parser->lexer, "E_COMMA_AF_BINOP");
        return instr;
    }


    instr.o2 = parser_parse_op(parser);

    printf("%ld, %ld\n", instr.size, instr.type);

    return instr;
}


PrimInstrDefine parser_parse_define(Parser* parser) {
    parser_consume(parser);

    PrimInstrDefine instr;

    if (!parser_expect(parser, TOK_AT)) {
        REPORT_ERROR(parser->lexer, "E_SYMBOL_AF_DEFINEKW");
        instr.id = "\0";
        return instr;
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
        instr.id = "\0";
        return instr;
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
            instr.id = "\0";
            return instr;
        }

        instr.args.id[instr.args.size] = parser->cur->value;

        if (parser->cur->type == TOK_COMMA) {
            instr.args.size++;
            instr.args.id = realloc(instr.args.id, sizeof(char*) * instr.args.size);
            instr.args.type = realloc(instr.args.id, sizeof(int) * instr.args.size);
        }


    }

    parser_consume(parser);

    return instr;
}