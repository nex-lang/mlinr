#ifndef TOKEN_H
#define TOKEN_H

#define NO_OF_KEYWORDS 52
#define KEYWORDS keywords
#define MAX_KEYWORD_LEN 14

#define MAX_IDENTIFIER_LEN 81

#define MAX_FLOAT_LIT_DIGITS 9
#define MAX_DOUBLE_LIT_DIGITS 17

#define IS_TYPEKW(type) \
    (((type) >= TOK_I8 && (type) <= TOK_U64))

#define IS_LITERAL(type) \
    (((type) >= TOK_L_SSINT && (type) <= TOK_L_SIZE))

typedef struct Token {
    unsigned int line, col;
    char* value;
    enum TokenType {
        // Special tokens
        TOK_ERROR,              // Error token
        TOK_EOF,                // End-of-file token (\0)
        TOK_IDEN,               // Identifier token (alphabets, digits, underscore)
        TOK_END_INSTR,               // \n

        // Symbols
        TOK_LPAREN,             // (
        TOK_RPAREN,             // )
        TOK_LBRACE,             // {
        TOK_RBRACE,             // }
        TOK_LBRACK,             // [
        TOK_RBRACK,             // ]
        TOK_PER,                // %
        TOK_AT,                 // @
        TOK_PTR,                // *
        TOK_COMMA,              // ,
        TOK_EQ,                 // =
        TOK_COLON,              // :

        // Keywords
        TOK_ADD,            // add (binary operation)
        TOK_ALLOCA,         // alloca (memory allocation instruction)
        TOK_BITCAST,        // bitcast (bitwise cast)
        TOK_BR,             // br (branch instruction)
        TOK_CALL,           // call (function call instruction)
        TOK_CAST,           // cast (cast operation)
        TOK_COND,           // cond (conditional branch)
        TOK_DECLARATION,    // declaration (generic, used for variable and function declarations)
        TOK_DEFINE,        // define (function definition)
        TOK_DIV,            // div (binary operation)
        TOK_DOUBLE,         // double (double precision floating point type)
        TOK_FPEXT,          // fpext (float-to-int extension cast)
        TOK_FPTOUI,         // fptoui (float-to-unsigned int cast)
        TOK_FPTOSI,         // fptosi (float-to-signed int cast)
        TOK_FLOAT,          // float (floating point type)
        TOK_FUNC,           // function (function type)
        TOK_GETELEMENTPTR,  // getelementptr (pointer arithmetic instruction)
        TOK_GE,             // ge (comparison operation)
        TOK_GLOBAL,        // global (for global variables)
        TOK_I1,            // i1 (1-bit integer type)
        TOK_I8,            // i8 (8-bit integer type)
        TOK_I16,           // i16 (16-bit integer type)
        TOK_I32,           // i32 (32-bit integer type)
        TOK_I64,           // i64 (64-bit integer type)
        TOK_U8,            // u8 (8-bit unsigned integer type)
        TOK_U16,           // u16 (16-bit unsigned integer type)
        TOK_U32,           // u32 (32-bit unsigned integer type)
        TOK_U64,           // u64 (64-bit unsigned integer type)
        TOK_F16,           // f16  (half precision)
        TOK_F32,           // f32  (float precision)
        TOK_F64,           // f64  (double precision) 
        TOK_INVOKE,        // invoke (invoke instruction)
        TOK_LOAD,          // load (load instruction)
        TOK_LABEL,         // label (used for basic block labels)
        TOK_LE,            // le (comparison operation)
        TOK_MUL,           // mul (binary operation)
        TOK_NE,            // ne (comparison operation)
        TOK_NSWITCH,       // switch (switch instruction)
        TOK_PHINODE,       // phi (PHI node)
        TOK_POINTER,       // pointer (pointer type)
        TOK_RETURN,        // return (return instruction)
        TOK_RESUME,        // resume (resume instruction)
        TOK_SEXT,          // sext (sign extension cast)
        TOK_STORE,         // store (store instruction)
        TOK_STRUCT,        // struct (structure type)
        TOK_SWITCH,        // switch (switch instruction)
        TOK_TRUNC,         // trunc (truncate cast)
        TOK_TYPE,         // type
        TOK_UITOFP,        // uitofp (unsigned int-to-float cast)
        TOK_UNION,         // union (union type)
        TOK_UNREACHABLE,   // unreachable (unreachable instruction)
        TOK_ZEXT,          // zext (zero extension cast)


        // Literals
        TOK_L_SSINT,            // 8-bit signed integer
        TOK_L_SINT,             // 16-bit signed integer
        TOK_L_INT,              // 32-bit signed integer
        TOK_L_LINT,             // 64-bit signed integer
        TOK_L_LLINT,            // 128-bit signed integer
        TOK_L_SSUINT,           // 8-bit unsigned integer
        TOK_L_SUINT,            // 16-bit unsigned integer
        TOK_L_UINT,             // 32-bit unsigned integer
        TOK_L_LUINT,            // 64-bit unsigned integer
        TOK_L_LLUINT,           // 128-bit unsigned integer
        TOK_L_FLOAT,            // 32-bit floating-point
        TOK_L_DOUBLE,           // 64-bit double-precision floating-point
        TOK_L_CHAR,             // 8-bit character
        TOK_L_STRING,           // Dynamic array of characters (string)
        TOK_L_BOOL,             // 8-bit boolean
        TOK_L_SIZE              // System-specific bit-size
    } type;
} Token;

#endif // TOKEN_H