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
                parser->tree->right = parser_parse_pinstruction(parser);
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

    return;
}


AST_Block parser_parse_instructions(Parser* parser) {
    AST_Block block;
    AST_Instruction instr;

    block.instructions = malloc(sizeof(AST_Instruction));
    block.size = 0;

    // printf("! %s\n", parser->cur->value);

    while (1) {
        printf("! %s %i %i\n", parser->cur->value, parser->cur->type, TOK_RBRACK);
        if (parser->cur->type == TOK_RBRACE || parser->cur->type == TOK_EOF) {
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


    parser_expect(parser, TOK_RBRACE);

    return block;
}


AST_Node* parser_parse_pinstruction(Parser* parser) {
    AST_Node* node = ast_init(PRIM_INSTRUCTION);

    node->data.pinstruction.type = PINSTR_DEFINE;            
    node->data.pinstruction.data.define = parser_parse_define(parser);            

    return node;
}


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
        op.type = OPERAND_ERR;
        return op;
    }

    if (parser->cur->type != TOK_IDEN) {
        op.type = OPERAND_ERR;
        return op; 
    }


    op.type = OPERAND_VARIABLE;
    
    Symbol* symb = symtbl_lookup(parser->tbl, parser->cur->value, parser->scope);
    if (symb == NULL) {
        op.type = OPERAND_ERR;
        REPORT_ERROR(parser->lexer, "U_USOUDV");
        return op;
    }

    op.value.variable = symb->id;

    parser_consume(parser);

    return op;
}


AST_Instruction parser_parse_instruction(Parser* parser) {
    /*
    Parses assignment, alloca, binary op and return instructions
    */

    AST_Instruction instr;

    if (IS_PBOP(parser->cur->type)) {
        instr.type = INSTR_BINARY_OP;
        instr.data.bin =  parser_parse_binop(parser);
        return instr;
    }

    switch (parser->cur->type) {

    case TOK_ALLOCA:
        instr.type = INSTR_ALLOCA;
        instr.data.alloca = parser_parse_alloca(parser);
        break;
    case TOK_RETURN:
        instr.type = INSTR_RETURN;
        instr.data.ret = parser_parse_ret(parser);
        break;
    case TOK_PER:
        instr.type = INSTR_ASSGN;
        instr.data.assgn = parser_parse_assgn(parser);
        break;
    case TOK_STORE:
        instr.type = INSTR_STORE;
        instr.data.store = parser_parse_store(parser);
        break;
    case TOK_LOAD:
        instr.type = INSTR_LOAD;
        instr.data.load = parser_parse_load(parser);
        break;
    default:
        break;
    }

    return instr;
}


InstrAlloca parser_parse_alloca(Parser* parser) {
    /*
    alloca <type> [, <num elements>] [, align <alignment>]
    */

    parser_consume(parser);

    InstrAlloca instr = (InstrAlloca){0};

    if (!IS_TYPEKW(parser->cur->type)) {
        REPORT_ERROR(parser->lexer, "E_TYPE_ALLOCA");
        return instr;
    }

    uint8_t type = parser->cur->type;
    parser_consume(parser);

    if (!parser_expect(parser, TOK_COMMA)) {
        instr.type = ALLOCA_VAL;
        instr.data.var = type;
        return instr;
    }

    if (parser_expect(parser, TOK_ALIGN)) {
        instr.type = ALLOCA_ALVAL;
        instr.data.alvar.type = type;

        instr.data.alvar.aln = parser_parse_ulit(parser, "align for alloca");

        if (instr.data.alvar.aln == 0) {
            return instr;
        }
    }

    if (!IS_TYPEKW(parser->cur->type)) {
        REPORT_ERROR(parser->lexer, "E_TorA_ALLOCA");
        return instr;
    }

    uint8_t arrtype = parser->cur->type;
    parser_consume(parser);
    uint64_t val = parser_parse_ulit(parser, "array for alloca");

    if (val == 0) {
        return instr;
    }

    if (!parser_expect(parser, TOK_COMMA)) {
        instr.type = ALLOCA_ARRAY;
        instr.data.array.type = type;
        instr.data.array.arrtype = arrtype;
        instr.data.array.val = val;

        return instr;
    }

    if (parser_expect(parser, TOK_ALIGN)) {
        instr.type = ALLOCA_ALARRAY;
        instr.data.alarray.type = type;
        instr.data.alarray.arrtype = arrtype;
        instr.data.alarray.val = val;

        instr.data.alarray.aln = parser_parse_ulit(parser, "align for array alloca");

        if (instr.data.alarray.aln == 0) {
            return instr;
        }

        return instr;
    }

    REPORT_ERROR(parser->lexer, "UATP_ALLOCA");
    return instr;
}


uint64_t parser_parse_ulit(Parser* parser, char* use) {
    AST_Operand op = parser_parse_op(parser);
        
    if (op.type != OPERAND_LITERAL) {
        REPORT_ERROR(parser->lexer, "E_VALID", use);
        return 0;
    }
    
    if (op.value.literal.type != TOK_L_SSUINT && op.value.literal.type != TOK_L_SUINT
    && op.value.literal.type != TOK_L_UINT && op.value.literal.type != TOK_L_LUINT) {
        REPORT_ERROR(parser->lexer, "E_UINT", use);
        return 0;
    }

    if (op.value.literal.value.uint == 0) {
        REPORT_ERROR(parser->lexer, "U_ZERO", use);
        return 0;
    }

    return op.value.literal.value.uint;
}


InstrBinOp parser_parse_binop(Parser* parser) {
    /*
    <pre_op> op <result type> <o1>, <o2>
    */


    InstrBinOp instr = (InstrBinOp){0};
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

    instr.size = type_to_size(parser->cur->type);
    parser_consume(parser);

    instr.o1 = parser_parse_op(parser);


    if (!parser_expect(parser, TOK_COMMA)) {
        REPORT_ERROR(parser->lexer, "E_COMMA_AF_BINOP");
        return instr;
    }


    instr.o2 = parser_parse_op(parser);

    // printf("BINOP: %ld with op1 %d and op2 %ld returning %ld\n", instr.size, instr.o1.value.variable, instr.o2.value.literal.value.int_, instr.type);

    return instr;
}

InstrAssign parser_parse_assgn(Parser* parser) {
    /*
    Handles assignment instructions %var = (instruction)
    */

    parser_consume(parser);

    InstrAssign instr = (InstrAssign){0};
    Symbol* sym = NULL;
    
    if (parser->cur->type != TOK_IDEN) {
        return instr;
    }

    char* iden = parser->cur->value;
    parser_consume(parser);
    
    if (!parser_expect(parser, TOK_SEQ)) {
        return instr;
    }

    instr.instr = malloc(sizeof(AST_Instruction));
    *instr.instr = parser_parse_instruction(parser);

    if (instr.instr->type == INSTR_BINARY_OP) {
        instr.size = type_to_size(instr.instr->data.bin.type);
    }

    if (instr.instr->type == INSTR_ALLOCA) {
        sym = symbol_init(iden, SYMBOL_VARIABLE, parser->scope, instr.size);
        symtbl_insert(parser, sym, iden);

        instr.iden = sym->id;

        return instr;
    }

    sym = symtbl_lookup(parser->tbl, iden, parser->scope);
    
    if (sym == NULL) {
        REPORT_ERROR(parser->lexer, "U_UDEFVA");
        return instr;
    }

    instr.iden = sym->id;

    return instr;
}

InstrReturn parser_parse_ret(Parser* parser) {
    parser_consume(parser);

    InstrReturn instr = (InstrReturn){0};


    if (!IS_TYPEKW(parser->cur->type)) {
        instr.val.type = OPERAND_VOID;
        return instr;
    }


    instr.type = parser->cur->type;
    
    parser_consume(parser);
    
    instr.val =  parser_parse_literal(parser);
    
    return instr;
}

InstrStore parser_parse_store(Parser* parser) {
    parser_consume(parser);

    InstrStore instr = (InstrStore){0};

    if (!IS_TYPEKW(parser->cur->type)) {
        REPORT_ERROR(parser->lexer, "E_TKW");
        return instr;
    }

    instr.type = parser->cur->type;
    parser_consume(parser);

    instr.push_op = parser_parse_op(parser);
    if (instr.push_op.type == OPERAND_ERR || instr.push_op.type == OPERAND_VOID) {
        REPORT_ERROR(parser->lexer, "I_PUSHOP");
        return instr;
    }

    if (!parser_expect(parser, TOK_COMMA)) {
        REPORT_ERROR(parser->lexer, "E_COMMA");
        return instr;
    }

    if (!IS_TYPEKW(parser->cur->type)) {
        REPORT_ERROR(parser->lexer, "E_TKW");
        return instr;
    }

    instr.ptr_type = parser->cur->type;
    parser_consume(parser);


    AST_Operand op = parser_parse_op(parser);
    if (op.type != OPERAND_VARIABLE) {
        REPORT_ERROR(parser->lexer, "I_PTROP");
        return instr;
    }

    instr.ptr_op = op.value.variable;

    if (!parser_expect(parser, TOK_COMMA)) {
        return instr;
    }
    

    if (!parser_expect(parser, TOK_ALIGN)) {
        REPORT_ERROR(parser->lexer, "E_ALIGN");
        return instr;
    }

    instr.aln = parser_parse_ulit(parser, "store");
    
    if (instr.aln == 0) {
        REPORT_ERROR(parser->lexer, "E_PROP_ALIGN");
        return instr;
    }

    return instr;
}

InstrLoad parser_parse_load(Parser* parser) {
    parser_consume(parser);

    InstrLoad instr = (InstrLoad){0};

    if (!IS_TYPEKW(parser->cur->type)) {
        REPORT_ERROR(parser->lexer, "E_TKW");
        return instr;
    }

    instr.type = type_to_size(parser->cur->type);
    parser_consume(parser);

    if (!parser_expect(parser, TOK_COMMA)) {
        REPORT_ERROR(parser->lexer, "E_COMMA");
        return instr;
    }

    if (!IS_TYPEKW(parser->cur->type)) {
        REPORT_ERROR(parser->lexer, "E_TKW");
        return instr;
    }

    instr.ptr_type = type_to_size(parser->cur->type);
    parser_consume(parser);


    AST_Operand op = parser_parse_op(parser);
    if (op.type != OPERAND_VARIABLE) {
        REPORT_ERROR(parser->lexer, "I_PTROP");
        return instr;
    }

    instr.ptr_op = op.value.variable;

    if (!parser_expect(parser, TOK_COMMA)) {
        return instr;
    }
    

    if (!parser_expect(parser, TOK_ALIGN)) {
        REPORT_ERROR(parser->lexer, "E_ALIGN");
        return instr;
    }

    instr.aln = parser_parse_ulit(parser, "load");
    
    if (instr.aln == 0) {
        REPORT_ERROR(parser->lexer, "E_PROP_ALIGN");
        return instr;
    }

    return instr;
}

PrimInstrDefine parser_parse_define(Parser* parser) {    
    parser_consume(parser);


    PrimInstrDefine instr;

    if (IS_TYPEKW(parser->cur->type)) {
        instr.type = parser->cur->type;
        parser_consume(parser);
    } else {
        instr.type = 0;
    }



    if (!parser_expect(parser, TOK_AT)) {
        REPORT_ERROR(parser->lexer, "E_SYMBOL_AF_DEFINEKW");
        instr.id = "\0";
        return instr;
    }

    if (parser->cur->type == TOK_IDEN) {
        instr.id = parser->cur->value;
        parser_consume(parser);
    }

    if (!parser_expect(parser, TOK_LPAREN)) {
        REPORT_ERROR(parser->lexer, "E_SYMBOL_PARAMS");
        instr.id = "\0";
        return instr;
    }

    ES(parser);

    instr.args.id = malloc(sizeof(char*));
    instr.args.type = malloc(sizeof(uint8_t));
    instr.args.size = 0;

    while (parser->cur->type != TOK_RPAREN) {
        if (IS_TYPEKW(parser->cur->type)) {
            instr.args.type[instr.args.size] = parser->cur->type;
            parser_consume(parser);
        }

        if (parser->cur->type == TOK_PER) {
            parser_consume(parser);
        }

        if (parser->cur->type != TOK_IDEN) {
            REPORT_ERROR(parser->lexer, "E_PARAM_ID");
        }

        symtbl_insert(parser, symbol_init(
            parser->cur->value, SYMBOL_VARIABLE, parser->scope, 
            type_to_size(instr.args.type[instr.args.size])),
            parser->cur->value
        );

        parser_consume(parser);

        if (parser->cur->type == TOK_COMMA) {
            parser_consume(parser);
            instr.args.size++;
            instr.args.id = realloc(instr.args.id, sizeof(char*) * instr.args.size);
            instr.args.type = realloc(instr.args.id, sizeof(uint8_t) * instr.args.size);
        }
    }


    parser_consume(parser);

    if (!parser_expect(parser, TOK_LBRACE)) {
        REPORT_ERROR(parser->lexer, "E_BLOCK_AF_DEF");
        instr.id = "\0";
        return instr;
    }


    instr.block = parser_parse_instructions(parser);    


    return instr;
}

size_t type_to_size(unsigned int type) {
    switch (type) {
        case TOK_I1:
        case TOK_I8:
        case TOK_U8:
            return 1;
        case TOK_I16:
        case TOK_U16:
        case TOK_F16:
            return 2;
        case TOK_I32:
        case TOK_U32:
        case TOK_F32:
            return 4;
        case TOK_I64:
        case TOK_U64:
        case TOK_F64:
            return 8;
        default:
            return 0;
    }
}


void symtbl_insert(Parser* parser, Symbol* symbol, char* raw_symb) {
    if (!parser->tbl) {
        exit(EXIT_FAILURE);
        return;
    }

    Symbol* checks = parser->tbl->symbol;
    printf("%s\n", raw_symb);
    while (checks != NULL) {
        if (checks->id == symbol->id) {
            REPORT_ERROR(parser->lexer, "U_REDEF", raw_symb);
            return;
        }
        checks = checks->next;
    }


    if (parser->tbl->symbol == NULL) {
        parser->tbl->symbol = symbol;
        return;
    }

    Symbol* current = parser->tbl->symbol;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = symbol;
}