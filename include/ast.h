#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct AST_Node AST_Node;
typedef struct AST_Module AST_Module;
typedef struct AST_Instruction AST_Instruction;
typedef struct AST_Operand AST_Operand;
typedef struct AST_Block AST_Block;
typedef struct AST_PrimInstruction AST_PrimInstruction;

typedef struct AST_Block {
    AST_Instruction* instructions;
    size_t size;
} AST_Block;

typedef struct PrimInstrDefine {
    char* id;
    int type;

    struct args {
        size_t size;
        char** id;
        int* type;
    } args;

    AST_Block block; // This can now be properly resolved
} PrimInstrDefine;

typedef struct AST_PrimInstruction {
    enum {
        PINSTR_DEFINE,
    } type;

    union {
        PrimInstrDefine define;
    } data;
} AST_PrimInstruction;

typedef struct AST_Module {
    char* identifier;
    struct {
        AST_PrimInstruction** instr;
        size_t size;
    } functions;
} AST_Module;

typedef struct AST_Literal {
    int type;
    union {
        int64_t int_;
        uint64_t uint;
        struct {
            float bit32;
            double bit64;
        } float_;
        char character;
        char* string;
    } value;
} AST_Literal;

typedef struct AST_Operand {
    enum {
        OPERAND_LITERAL,
        OPERAND_VARIABLE,
    } type;

    union {
        AST_Literal literal;
        char* variable;
    } value;
} AST_Operand;

typedef union InstrAlloca {
    struct {
        AST_Operand n;
        size_t unit;
    } mult;

    AST_Operand sg;
} InstrAlloca;

typedef struct InstrBinOp {
    size_t type, size;

    AST_Operand o1;
    AST_Operand o2;
} InstrBinOp;

typedef struct AST_Instruction {
    enum {
        INSTR_BINARY_OP,
        INSTR_DEFINE,
        INSTR_RETURN,
        INSTR_CALL,
        INSTR_ALLOCA
    } type;

    union {
        InstrBinOp bin;

        struct {
            AST_Operand* ret_val;
        } ret;

        struct {
            char* function_name;
            struct {
                AST_Operand** args;
                size_t size;
            } args;
        } call;

        InstrAlloca alloca;
    } data;
} AST_Instruction;

typedef enum NodeTypes {
    MODULE,
    INSTRUCTION,
    PRIM_INSTRUCTION,
    ROOT
} NodeTypes;

struct AST_Node {
    NodeTypes type;

    union {
        AST_Module module;
        AST_PrimInstruction pinstruction;
        AST_Instruction instruction;
    } data;

    AST_Node* right;
    AST_Node* left;
};

AST_Node* ast_init(NodeTypes type);
void ast_free(AST_Node* node);

#endif // AST_H