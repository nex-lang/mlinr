#ifndef TOKEN_H
#define TOKEN_H

#define NO_OF_KEYWORDS 67
#define KEYWORDS keywords
#define MAX_KEYWORD_LEN 14

#define MAX_IDENTIFIER_LEN 81

#define MAX_FLOAT_LIT_DIGITS 9
#define MAX_DOUBLE_LIT_DIGITS 17

#define IS_TYPEKW(type) \
    (((type) >= TOK_I1 && (type) <= TOK_U64))

#define IS_PBOP(type) \
    (((type) >= TOK_ADD && (type) <= TOK_FCMP))


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
        TOK_SEQ,                 // =
        TOK_COLON,              // :

        // Keywords
        TOK_ADD,            // add (binary addition operation)
        TOK_SUB,            // sub (binary subtraction operation)
        TOK_MUL,            // mul (binary multiplication operation)
        TOK_SDIV,           // sdiv (signed division operation)
        TOK_UDIV,           // udiv (unsigned division operation)
        TOK_AND,            // and (logical AND operation)
        TOK_OR,             // or (bitwise OR operation)
        TOK_XOR,            // xor (bitwise XOR operation)
        TOK_SHL,            // shl (bitwise shift left operation)
        TOK_SHR,            // shr (bitwise shift right operation)
        TOK_EQ,             // eq (equal comparison operation)
        TOK_NE,             // ne (not equal comparison operation)
        TOK_GT,             // gt (greater than comparison operation)
        TOK_GE,             // ge (greater than or equal comparison operation)
        TOK_LT,             // lt (less than comparison operation)
        TOK_LE,             // le (less than or equal comparison operation)

        TOK_ICMP,           // icmp (compares passed condition against its int operands)
        TOK_FCMP,           // fcmp (compares passed condition against its float operands)

        TOK_ALLOCA,         // alloca (memory allocation instruction)
        TOK_BITCAST,        // bitcast (bitwise cast)
        TOK_BR,             // br (branch instruction)
        TOK_CALL,           // call (function call instruction)
        TOK_CAST,           // cast (cast operation)
        TOK_COND,           // cond (conditional branch)
        TOK_DECLARATION,    // declaration (generic, used for variable and function declarations)
        TOK_DEFINE,         // define (function definition)
        TOK_DOUBLE,         // double (double precision floating point type)
        TOK_FPEXT,          // fpext (float-to-int extension cast)
        TOK_FPTOUI,         // fptoui (float-to-unsigned int cast)
        TOK_FPTOSI,         // fptosi (float-to-signed int cast)
        TOK_FLOAT,          // float (floating point type)
        TOK_FUNC,           // function (function type)
        TOK_GLOBAL,         // global (for global variables)
        TOK_GETELEMENTPTR,  // getelementptr (pointer arithmetic instruction)
        TOK_I1,             // i1 (1-bit integer type)
        TOK_I8,             // i8 (8-bit integer type)
        TOK_I16,            // i16 (16-bit integer type)
        TOK_I32,            // i32 (32-bit integer type)
        TOK_I64,            // i64 (64-bit integer type)
        TOK_F16,            // f16 (half precision)
        TOK_F32,            // f32 (float precision)
        TOK_F64,            // f64 (double precision)
        TOK_U8,             // u8 (8-bit unsigned integer type)
        TOK_U16,            // u16 (16-bit unsigned integer type)
        TOK_U32,            // u32 (32-bit unsigned integer type)
        TOK_U64,            // u64 (64-bit unsigned integer type)
        TOK_LAND,           // land (logical AND operation)
        TOK_LNAND,          // lnand (logical NAND operation)
        TOK_LNOR,           // lnor (logical NOR operation)
        TOK_LNOT,           // lnot (logical NOT operation)
        TOK_LOR,            // lor (logical OR operation)
        TOK_LOAD,           // load (load instruction)
        TOK_LABEL,          // label (used for basic block labels)
        TOK_NSWITCH,        // switch (switch instruction)
        TOK_PHINODE,        // phi (PHI node)
        TOK_POINTER,        // pointer (pointer type)
        TOK_RETURN,         // return (return instruction)
        TOK_RESUME,         // resume (resume instruction)
        TOK_STORE,          // store (store instruction)
        TOK_STRUCT,         // struct (structure type)
        TOK_SEXT,           // sext (sign extension cast)
        TOK_SWITCH,         // switch (switch instruction)
        TOK_TRUNC,          // trunc (truncate cast)
        TOK_TYPE,           // type
        TOK_UITOFP,         // uitofp (unsigned int-to-float cast)
        TOK_UNION,          // union (union type)
        TOK_UNREACHABLE,    // unreachable (unreachable instruction)
        TOK_ZEXT,            // zext (zero extension cast)


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
        TOK_L_SIZE,              // System-specific bit-size

        TOK_fEQ, TOK_fNE, TOK_fGT, TOK_fGE, TOK_fLT, TOK_fLE

    } type;
} Token;

#endif // TOKEN_H