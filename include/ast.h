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

typedef struct PrimInstrDecl {
    char* id;
    int type;

    struct {
        size_t size;
        int32_t* id;
        uint8_t* type;
    } args; 
} PrimInstrDecl;

typedef struct PrimInstrDefine {
    char* id;
    int type;

    struct {
        size_t size;
        int32_t* id;
        uint8_t* type;
    } args;

    AST_Block block;
} PrimInstrDefine;

typedef struct AST_PrimInstruction {
    enum {
        PINSTR_ROOT,
        PINSTR_DEFINE,
        PINSTR_DECL
    } type;

    union {
        PrimInstrDefine define;
        PrimInstrDecl decl;
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
        OPERAND_VOID,
        OPERAND_ERR
    } type;

    union {
        AST_Literal literal;
        int32_t variable;
    } value;
} AST_Operand;

typedef struct InstrBinOp {
    size_t type, size;

    AST_Operand o1;
    AST_Operand o2;
} InstrBinOp;

typedef struct InstrAlloca {
    enum {
        ALLOCA_VAL,
        ALLOCA_ALVAL,
        ALLOCA_ARRAY,
        ALLOCA_ALARRAY,
    } type;
    
    union {
        uint8_t var;
        struct {
            uint64_t aln;
            uint8_t type;
        } alvar;

        struct {
            uint8_t type;
        
            uint8_t arrtype;
            uint64_t val;
        } array;

        struct {            
            uint8_t type;
        
            uint8_t arrtype;
            uint64_t val;
                    
            uint64_t aln;
        } alarray;
    } data;

    uint64_t off;
    uint64_t size;
} InstrAlloca;

typedef struct InstrReturn {
    size_t type;
    AST_Literal val;
} InstrReturn;

typedef struct InstrCall {
    char* iden;
    uint8_t type;

    struct {
        AST_Operand* args;
        uint32_t* sizes;
        size_t size;
    } args;
} InstrCall;

typedef struct InstrAssign {
    int32_t iden;
    size_t size;
    AST_Instruction* instr;
} InstrAssign;

typedef struct InstrStore {
    size_t type;
    AST_Operand push_op;

    size_t ptr_type;
    int32_t ptr_op;

    uint64_t aln;
} InstrStore;

typedef struct InstrLoad {
    size_t type;

    size_t ptr_type;
    int32_t ptr_op;

    uint64_t aln;
} InstrLoad;

typedef struct InstrGEP {

} InstrGEP;

typedef struct AST_Instruction {
    enum {
        INSTR_BINARY_OP,
        INSTR_RETURN,
        INSTR_CALL,
        INSTR_ALLOCA,
        INSTR_ASSGN,
        INSTR_STORE,
        INSTR_LOAD
    } type;

    union {
        InstrBinOp bin;
        InstrReturn ret;
        InstrCall call;
        InstrAlloca alloca;
        InstrAssign assgn;
        InstrStore store;
        InstrLoad load;
        InstrGEP gep;
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