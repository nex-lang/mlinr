#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


char* keywords[NO_OF_KEYWORDS] = {
    "align", "add", "alloca", "bitcast", "br", "call", "cast", "cond", "declaration", "define", "double",
    "eq", "fpext", "fptoui", "fptosi", "float", "function", "ge", "gt", "global", "getelementptr", "i1", "i8",
    "i16", "i32", "i64", "f16", "f32", "f64", "u8", "u16", "u32", "u64", "land", "lnand", "lnor", "lnot",
    "lor", "load", "label", "le", "lt", "mul", "ne", "nswitch", "or", "phinode", "pointer",
    "ret", "resume", "sdiv", "sext", "shl", "shr", "sub", "store", "struct", "switch", "trunc", "type",
    "uitofp", "union", "unreachable", "udiv", "xor", "zext", "icmp", "fcmp"
};


Lexer* lexer_init(char* filename) {
    /*
    Initializes lexer to lex provided values
    return: pointer to a propperly initalized lexer struct
    */

    Lexer* lexer = calloc(1, sizeof(Lexer));

    if (!lexer) {
        exit(EXIT_FAILURE);
    }

    lexer->buf = io_load_file(filename);
    lexer->buf_size = strlen(lexer->buf);


    lexer->i = 0;
    lexer->c = lexer->buf[lexer->i];
    
    lexer->cc = 1;
    lexer->cl = 1;

    return (Lexer*)lexer;
}

void lexer_free(Lexer* lexer) {
    /*
    De-initializes provided lexer
    */

    if (!lexer) {
        return;
        exit(EXIT_FAILURE);
    }

    free(lexer->buf);
    free(lexer);

    lexer = NULL;
}

Token* lexer_token_init(Lexer* lexer, char* value, uint8_t type) {
    /*
    Initializes token with provided values 
    return: pointer to a propperly initalized token struct
    */

    Token* token = calloc(1, sizeof(Token));

    if (!token) {
        exit(EXIT_FAILURE);
    }

    token->type = type;  
    token->col = lexer->cc;
    token->line = lexer->cl;

    token->value = strdup(value);

    return (Token*)token;
}

void token_free(Token* token) {
    /*
    De-initializes provided token
    */

    if (!token) {
        exit(EXIT_FAILURE);
        return;
    }

    free(token->value);
    free(token);

    token = NULL;
}


Token* lexer_next_token(Lexer* lexer) {
    Token* token;
    lexer_handle_fillers(lexer);

    if (lexer->c == '\0' || (lexer->i + 1) >= lexer->buf_size) { 
        token = lexer_token_init(lexer, "\0", TOK_EOF);
    } else if (isalpha(lexer->c) || lexer->c == '_') {
        token = lexer_handle_alpha(lexer);
    } else if (isdigit(lexer->c)) {
        token = lexer_handle_numeric(lexer, false);
    } else {
        token = lexer_handle_1char(lexer);
        if (token->type == TOK_ERROR) {
            lexer_handle_error(lexer);
            return lexer_next_token(lexer);
        }
    }

    return token;
}

char* lexer_peek(Lexer* lexer, int8_t offset) {
    /*
    Extracts provided # of characters ahead of the current lexer position
    Returns: NULL (EOF), EXIT_FAILURE (alloc error), char* (valid peek)
    */


    if (offset < 0 && (size_t)(-offset) > lexer->i) {
        return NULL;
    }

    size_t peek_start = (size_t)(offset < 0 ? lexer->i + offset : lexer->i);
    size_t peek_length = (size_t)(offset < 0 ? -offset : offset);

    if (peek_start >= lexer->buf_size || peek_start + peek_length > lexer->buf_size) {
        return NULL;
    }

    char* info = malloc((peek_length + 1) * sizeof(char));

    if (!info) {
        exit(EXIT_FAILURE);
    }

    strncpy(info, lexer->buf + peek_start, peek_length);
    info[peek_length] = '\0';

    return info;
}

char lexer_peep(Lexer* lexer, int8_t offset) {
    /*
    Extracts character of position of the provided # from the current lexer position
    Returns: '\0' (EOF), char (valid peek)
    */

   size_t peep_index = (size_t)(lexer->i + offset);

    if (peep_index >= lexer->buf_size || peep_index < 0) {
        return '\0';
    }

    return lexer->buf[peep_index];
}

void lexer_advance(Lexer* lexer, uint8_t offset) {
    /*
    Advances current lexer position by provided # of charachters; returns NULL if EOF
    */

    if (lexer->buf[lexer->i + offset] == '\0' || lexer->i + offset >= lexer->buf_size) {
        return;
    }

    lexer->i += offset;
    lexer->cc += 1;
    lexer->c = lexer->buf[lexer->i];
}

void lexer_handle_error(Lexer* lexer) {
    while (lexer->c != '\0' && lexer->c != ' ' && lexer->c != '\n' &&
           lexer->c != '\v' && lexer->c != '\t' && lexer->buf[lexer->i + 1] != '\0' &&
           lexer->i + 1 < lexer->buf_size) {
        lexer_advance(lexer, 1);
    }
}

void lexer_handle_fillers(Lexer* lexer) {
    /*
    Skips un-importaint values (whitespace, newline, horizontal/vertical tab)
    */

    switch (lexer->c) {
        case ' ':
            lexer_advance(lexer, 1);
            break;
        case '\t':
            lexer_advance(lexer, 1);
            break;
        case '\v':
        case '\n':
            lexer_advance(lexer, 1);
            lexer->cl += 1;
            lexer->cc = 1;
        default:
            break;
    }
}

Token* lexer_handle_alpha(Lexer* lexer) {
    /* 
    Identifies and creates TOK_IDEN or keyword tokens [TOK_IMPORT -> TOK_AS]
    return: identifiers and keywords
    */

    char* buf = calloc(1, sizeof(char));


    while (isalnum(lexer->c) || lexer->c == '_') {
        buf = realloc(buf, (strlen(buf) + 2) * sizeof(char));

        if (!buf) {
            exit(EXIT_FAILURE);
        }
        
        if (strlen(buf) > MAX_IDENTIFIER_LEN) {
            REPORT_ERROR(lexer, "E_SHORTER_LENIDEN", MAX_IDENTIFIER_LEN);
            lexer_handle_error(lexer);
            return lexer_next_token(lexer);
        }

        strcat(buf, (char[]){lexer->c, '\0'}); 
        lexer_advance(lexer, 1);
    } 



    if (strlen(buf) > MAX_KEYWORD_LEN) {
        // early check to avoid keyword checking loop
        return lexer_token_init(lexer, buf, TOK_IDEN);
    }

    uint8_t KWCHAR_TYPE_MAP[NO_OF_KEYWORDS] = {
        TOK_ALIGN, TOK_ADD, TOK_ALLOCA, TOK_BITCAST, TOK_BR, TOK_CALL, TOK_CAST, TOK_COND, 
        TOK_DECLARATION, TOK_DEFINE, TOK_DOUBLE, TOK_EQ, TOK_FPEXT, TOK_FPTOUI, 
        TOK_FPTOSI, TOK_FLOAT, TOK_FUNC, TOK_GE, TOK_GT, TOK_GLOBAL,
        TOK_GETELEMENTPTR, TOK_I1, TOK_I8, TOK_I16, TOK_I32, TOK_I64,
        TOK_F16, TOK_F32, TOK_F64, TOK_U8, TOK_U16, TOK_U32,
        TOK_U64, TOK_LAND, TOK_LNAND, TOK_LNOR, TOK_LNOT, TOK_LOR,
        TOK_LOAD, TOK_LABEL, TOK_LE, TOK_LT, TOK_MUL, TOK_NE, TOK_NSWITCH, TOK_OR,
        TOK_PHINODE, TOK_POINTER, TOK_RETURN, TOK_RESUME, TOK_SDIV, TOK_SEXT, TOK_SHL, TOK_SHR, TOK_SUB,
        TOK_STORE, TOK_STRUCT, TOK_SWITCH, TOK_TRUNC, TOK_TYPE, TOK_UITOFP, TOK_UNION,
        TOK_UNREACHABLE, TOK_UDIV, TOK_XOR, TOK_ZEXT, TOK_ICMP, TOK_FCMP
    };

    for (uint8_t i = 0; i < NO_OF_KEYWORDS - 1; i++) {
        if (strcmp(KEYWORDS[i], buf) == 0) {
            return lexer_token_init(lexer, buf, KWCHAR_TYPE_MAP[i]);
        }
    }

    return lexer_token_init(lexer, buf, TOK_IDEN);
}


Token* lexer_handle_numeric(Lexer* lexer, bool is_negative) {
    /*
    Identifies and creates numeric tokens 
    return: numeric tokens [TOK_L_SSINT-> TOK_L_DOUBLE]
    */
   
    int type = TOK_ERROR;
    __uint8_t npre_decimal = 0;
    char* buf = calloc(2, sizeof(char));

    if (!buf) {
        exit(EXIT_FAILURE);
    }

    if (is_negative) {
        // register sign
        strcat(buf, (char[]){'-', '\0'}); 
        lexer_advance(lexer, 1);
    }

    lexer_process_digits(lexer, &buf, false);
    type = lexer_process_int_type(buf);

    if (lexer->c == '.') {
        // register decimal
        buf = realloc(buf, (strlen(buf) + 2) * sizeof(char));
        
        if (!buf) {
            exit(EXIT_FAILURE);
        }

        strcat(buf, (char[]){lexer->c, '\0'});
        lexer_advance(lexer, 1);

        npre_decimal = strlen(buf); // # of digits before decimal

        lexer_process_digits(lexer, &buf, true);

        type = lexer_process_decimal_type(buf, (strlen(buf) - npre_decimal));
    }

    if (type == TOK_ERROR) {
        REPORT_ERROR(lexer, "U_NUM_LIT_TYPE");
        lexer_handle_error(lexer);
        return lexer_next_token(lexer);
    }

    return lexer_token_init(lexer, buf, type);
}

Token* lexer_handle_1char(Lexer* lexer) {
    /*
    Identifies and creates tokens for op-erator and checks for possible operator combinations
    return: [TOK_LPAREN->TOK_COLON], and literals
    */


    switch (lexer->c) {
        case '=':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "=", TOK_SEQ);
            break;
        case ',':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, ",", TOK_COMMA);
            break;
        case '%':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "%", TOK_PER);
            break;
        case '@':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "@", TOK_AT);
            break;
        case '*':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "*", TOK_PTR);
            break;
        case ':':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, ":", TOK_COLON);
            break;
        case ';':
            while (lexer->c != '\n') {
                lexer_advance(lexer, 1);
            }
            lexer_advance(lexer, 1);
            break;
        case '(':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "(", TOK_LPAREN);
            break;
        case ')':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, ")", TOK_RPAREN);
            break;
        case '{':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "{", TOK_LBRACE);
            break;
        case '}':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "}", TOK_RBRACE);
            break;
        case '[':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "[", TOK_LBRACK);
            break;
        case ']':
            lexer_advance(lexer, 1); return lexer_token_init(lexer, "]", TOK_RBRACK);
            break;
        case '\'':
            return lexer_process_single_quote(lexer);
            break;
        case '\"':
            return lexer_process_double_quote(lexer);
            break;
        default:
            break;
    }


    return lexer_next_token(lexer);
}

void lexer_process_digits(Lexer* lexer, char** buf, bool has_decimal) {

    while (isdigit(lexer->c) ||
           ((lexer->c == 'f' || lexer->c == 'F' || lexer->c == 'd' || lexer->c == 'D') && has_decimal == true)) {
        size_t len = strlen(*buf);
        *buf = realloc(*buf, (len + 2) * sizeof(char));

        if (!*buf) {
            exit(EXIT_FAILURE);
        }

        (*buf)[len] = lexer->c;
        (*buf)[len + 1] = '\0';


        lexer_advance(lexer, 1);
    }

    if (lexer->c == '_' || lexer->c == '\'') {
        if (isdigit(lexer_peep(lexer, -1)) && isdigit(lexer_peep(lexer, 1))) {
            lexer_advance(lexer, 1);
            lexer_process_digits(lexer, buf, has_decimal);
        } else {
            free(*buf);
            lexer_handle_error(lexer);
        }
    } else if (lexer->c == '.' && has_decimal) {
        free(*buf);
        lexer_handle_error(lexer);
    }
}


uint8_t lexer_process_decimal_type(char* buf, uint8_t diadc) {
    /*
    Processes provided buffer and resolves type; TOK_ERROR if invalid
    return: TOK_ERROR, TOK_L_FLOAT (32-bit float), or TOK_L_DOUBLE (64-bit float)
    */

    bool has_Fpfx = false; // has float prefix
    bool has_Dpfx = false; // has double prefix

    for (uint8_t i = 0; i < strlen(buf); i++) {
        if (buf[i] == 'f' || buf[i] == 'F') {
            if (has_Fpfx) { 
                return TOK_ERROR;
            }
            has_Fpfx = true;
        } else if (buf[i] == 'd' || buf[i] == 'D') {
            if (has_Dpfx) { 
                return TOK_ERROR;
            }
            has_Dpfx = true;
        }
    }

    if (has_Fpfx) {
        return TOK_L_FLOAT;
    } else if (has_Dpfx) {
        return TOK_L_DOUBLE;
    }

    if (diadc <= MAX_FLOAT_LIT_DIGITS) {
        return TOK_L_FLOAT;
    } else if (diadc <= MAX_DOUBLE_LIT_DIGITS) {
        return TOK_L_DOUBLE;
    }

    return TOK_ERROR;
}


int is_within_int_range(int128_t val, int128_t min, int128_t max) {
    if (val.high > max.high || (val.high == max.high && val.low > max.low)) {
        return 0;
    }
    if (val.high < min.high || (val.high == min.high && val.low < min.low)) {
        return 0;
    }
    return 1;
}

int is_within_uint_range(uint128_t val, uint128_t max) {
    if (val.high > max.high || (val.high == max.high && val.low > max.low)) {
        return 0;
    }
    return 1;
}

uint8_t lexer_process_int_type(char* buf) {
    char *endptr;
    int64_t signed_val;
    uint64_t usigned_val;

    buf[strlen(buf)] = '\0';

    if (*buf == '-') {
        int128_t lsigned_val;
        strtoint128(buf, lsigned_val);

        int128_t int128_max = { .high = 0x7FFFFFFFFFFFFFFF, .low = 0xFFFFFFFFFFFFFFFF };
        int128_t int128_min = { .high = 0x8000000000000000, .low = 0 };

        signed_val = strtoll(buf, &endptr, 10) - 1;

        if (endptr == buf || *endptr != '\0') {
            return TOK_ERROR;
        }

        if (is_within_int_range(lsigned_val, int128_min, int128_max)) {
            if (lsigned_val.high <= 0) {
                if (signed_val >= INT8_MIN && signed_val <= INT8_MAX) {
                    return TOK_L_SSINT;
                } else if (signed_val >= INT16_MIN && signed_val <= INT16_MAX) {
                    return TOK_L_SINT;
                } else if (signed_val >= INT32_MIN && signed_val <= INT32_MAX) {
                    return TOK_L_INT;
                } else if (signed_val >= INT64_MIN && signed_val <= INT64_MAX) {
                    return TOK_L_LINT;
                }
            } else if (lsigned_val.high > 0) {
                return TOK_L_LLINT;
            }
        }        
        return TOK_ERROR;
    } else {
        uint128_t unsigned_val;
        strtouint128(buf, unsigned_val);

        usigned_val = strtoull(buf, &endptr, 10);
        signed_val = strtoll(buf, &endptr, 10);

        if (endptr == buf || *endptr != '\0') {
            return TOK_ERROR;
        }

        uint128_t uint128_max = { .high = 0xFFFFFFFFFFFFFFFF, .low = 0xFFFFFFFFFFFFFFFF };

        if (is_within_uint_range(unsigned_val, uint128_max)) {
            if (unsigned_val.low > 0) {
                return TOK_L_LLUINT;
            } else if (usigned_val >= 0 && usigned_val <= UINT8_MAX) {
                return TOK_L_SSUINT;
            } else if (usigned_val >= 0 && usigned_val <= UINT16_MAX) {
                return TOK_L_SUINT;
            } else if (usigned_val >= 0 && usigned_val <= UINT32_MAX) {
                return TOK_L_UINT;
            } else if (usigned_val >= 0 && usigned_val <= UINT64_MAX) {
                return TOK_L_LUINT;
            }
        } else if (signed_val >= INT8_MIN && signed_val <= INT8_MAX) {
            return TOK_L_SSINT;
        } else if (signed_val >= INT16_MIN && signed_val <= INT16_MAX) {
            return TOK_L_SINT;
        } else if (signed_val >= INT32_MIN && signed_val <= INT32_MAX) {
            return TOK_L_INT;
        } else if (signed_val >= INT64_MIN && signed_val <= INT64_MAX) {
            return TOK_L_LINT;
        }
    }

    return TOK_ERROR;
}

Token* lexer_process_single_quote(Lexer* lexer) {
    lexer_advance(lexer, 1); // consume '

    char* value = calloc(2, sizeof(char));
    if (!value) {
        exit(EXIT_FAILURE);
    }
    value[0] = lexer->c;
    value[1] = '\0';

    lexer_advance(lexer, 1); // consume char

    if (lexer->c == '\'') {
        lexer_advance(lexer, 1); // consume '
        return lexer_token_init(lexer, value, TOK_L_CHAR);
    }

    REPORT_ERROR(lexer, "E_CHAR_TERMINATOR");
    lexer_handle_error(lexer);
    return lexer_next_token(lexer);
}

Token* lexer_process_double_quote(Lexer* lexer) {
    lexer_advance(lexer, 1); // consume "
    
    char* value = calloc(1, sizeof(char) + 1);

    if (!value) {
        exit(EXIT_FAILURE);
    }

    while (lexer->c != '"' && lexer->c != '\0') {
        value = realloc(value, (strlen(value) + 2) * sizeof(char));
            
        if (!value) {
            exit(EXIT_FAILURE);
        }
            
        strncat(value, &(lexer->c), 1);
        lexer_advance(lexer, 1); // consume char
    }

    if (lexer->c == '"') {
        lexer_advance(lexer, 1);
        return lexer_token_init(lexer, value, TOK_L_STRING);
    }

    REPORT_ERROR(lexer, "E_STRING_TERMINATOR");
    lexer_handle_error(lexer);
    return lexer_next_token(lexer);
}


struct ErrorTemplate templates[] = {
    {"U_NO_OF_DECIMAL", "Unexpected number of digits after decimal point - configuration expects: %d found: %d"},
    {"U_NUM_LIT_TYPE", "Unexpected numeric literal found - accepted formats: docs::literals::numeric"},
    {"U_DB_SOURCE_DECL", "Unexpected double source declration, first sourced: '%s' again: '%s' - expects: single 'from' statement"},
    {"U_MODULE_MULT_ALIAS", "Unexpected attempt to alias %d seperate identifiers into single '%s' - find proper syntax: docs::imports"},

    {"E_SHORTER_LENIDEN", "Expected a shorter identifier length - configuration expects: <= %d"},
    {"E_CHAR_TERMINATOR", "Expected a (') character literal terminator after starting of character literal"},
    {"E_STRING_TERMINATOR", "Expected a (\") string literal terminator after starting of string literal"},
    {"E_DTS_FN_PARAM", "Expected a valid data type specifier while specifying parameters for a function, '%s' needs a type"},
    {"E_MEP_MATCH_LBRACK", "Expected a '}' to match brackets for MEP, found '%s'"},
    {"E_PROP_EXP", "Expected a propper expression, got '%s'"},
    {"E_EQ_VAR_EXPR", "%zu variables cant be assigned with %zu expressions - to set all %zu variables to a singular value, use one expression (e.g. a, b = 1)"}
};

char* lexer_get_reference(Lexer* lexer) {
    size_t line_start = lexer->i;

    while (line_start > 0 && lexer->buf[line_start - 1] != '\n') {
        line_start--;
    }

    size_t line_end = lexer->i;

    while (lexer->buf[line_end] != '\0' && lexer->buf[line_end] != '\n') {
        line_end++;
    }

    size_t line_length = line_end - line_start;
    char* line_content = (char*)malloc((line_length + 2) * sizeof(char));

    if (!line_content) {
        free(line_content);
        exit(EXIT_FAILURE);
    }

    strncpy(line_content, lexer->buf + line_start, line_length);

    line_content[line_length] = '\0';

    return line_content;
}

void lexer_report_error(Lexer* lexer, char* error_code, ...) {
    if (error_code[0] != 'U' && error_code[0] != 'E') {
        return;
    }

    for (size_t i = 0; i < (sizeof(templates) / sizeof(templates[0])); i++) {        
        if (strcmp(templates[i].code, error_code) == 0) {
            va_list args;
            va_start(args, error_code);

            char* final_content = malloc(
                (strlen(templates[i].content) + sizeof(args)) * sizeof(char)
            );

            if (final_content != NULL) {
                vsprintf(final_content, templates[i].content, args);
            }

            va_end(args);

            char* refrence = lexer_get_reference(lexer);

            printf("[%d : %d] > %s\n\t%d | %s\n", lexer->cl, lexer->cc, final_content, lexer->cl, refrence);
            free(final_content);
            return;
        }
    }

    char* refrence = lexer_get_reference(lexer);
    printf("[%d : %d] > %s\n\t%d | %s\n", lexer->cl, lexer->cc, error_code, lexer->cl, refrence);
}