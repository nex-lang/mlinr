#ifndef AST_H
#define AST_H

#include <stdbool.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

// Forward declarations
typedef struct AST_Node AST_Node;
typedef struct AST_Module AST_Module;
typedef struct AST_Function AST_Function;
typedef struct AST_BasicBlock AST_BasicBlock;
typedef struct AST_Instruction AST_Instruction;
typedef struct AST_Operand AST_Operand;

typedef struct AST_Module {
    char* identifier;
    struct {
        AST_Function** functions;
        size_t size;
    } functions;
} AST_Module;

typedef struct AST_Function {
    char* identifier;
    char* return_type;
    struct {
        char** params;
        size_t size;
    } params;
    struct {
        AST_BasicBlock** basic_blocks;
        size_t size;
    } basic_blocks;
} AST_Function;

typedef struct AST_BasicBlock {
    char* identifier;
    struct {
        AST_Instruction** instructions;
        size_t size;
    } instructions;
} AST_BasicBlock;

typedef struct AST_Operand {
    enum {
        OPERAND_LITERAL,
        OPERAND_VARIABLE,
    } type;

    union {
        int64_t literal;
        char* variable;
    } value;
} AST_Operand;

typedef struct AST_Instruction {
    enum {
        INSTR_BINARY_OP,
        INSTR_RETURN,
        INSTR_CALL,
    } type;

    union {
        struct {
            char* op;
            AST_Operand* lhs;
            AST_Operand* rhs;
            char* result;
        } binary_op;

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
    } data;
} AST_Instruction;

typedef enum NodeTypes {
    MODULE,
    FUNCTION,
    BLOCK,
    INSTRUCTION,
} NodeTypes;


struct AST_Node {
    NodeTypes type;

    union {
        AST_Module module;
        AST_Function function;
        AST_BasicBlock basic_block;
        AST_Instruction instruction;
    } data;

    AST_Node* right;
    AST_Node* left;
};

AST_Node* ast_init(NodeTypes type);
void ast_free(AST_Node* node);

#endif // AST_H