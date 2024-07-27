#ifndef LEXER_H
#define LEXER_H

#include "io.h"
#include "token.h"
#include "bit128.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>


typedef struct Lexer {
    char c; // current charachter
    unsigned int i; // current index 

    char* buf; // buffer
    unsigned int buf_size; // buffer size 
     
    uint8_t cc; // current column
    unsigned int cl; // current line
} Lexer;

// allocating and freeing methods

Lexer* lexer_init(char* filename);
void lexer_free(Lexer* lexer);

Token* lexer_token_init(Lexer* lexer, char* value, uint8_t type);
void token_free(Token* token);

// important and general functions 

Token* lexer_next_token(Lexer* lexer);

char* lexer_peek(Lexer* lexer, int8_t offset);
char lexer_peep(Lexer* lexer, int8_t offset);
void lexer_advance(Lexer* lexer, uint8_t offset);

// helper functions

void lexer_handle_error(Lexer* lexer);
void lexer_handle_fillers(Lexer* lexer);

Token* lexer_handle_alpha(Lexer* lexer);
Token* lexer_handle_numeric(Lexer* lexer, bool is_negative);
Token* lexer_handle_1char(Lexer* lexer);

// helper-helper functions

void lexer_process_digits(Lexer* lexer, char** buf, bool has_decimal);
uint8_t lexer_process_decimal_type(char* buf, uint8_t diadc);
int is_within_int_range(int128_t val, int128_t min, int128_t max);
int is_within_uint_range(uint128_t val, uint128_t max);
uint8_t lexer_process_int_type(char* buf);


// error manager

#ifndef REPORT_ERROR
#define REPORT_ERROR lexer_report_error
#endif

struct ErrorTemplate {
    const char* code;
    const char* content;
};

extern struct ErrorTemplate templates[];

char* lexer_get_reference(Lexer* lexer);
void lexer_report_error(Lexer* lexer, char* error_code, ...);

#endif // LEXER_H